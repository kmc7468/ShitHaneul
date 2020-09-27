#include <ShitHaneul/Memory.hpp>

#include <ShitHaneul/Interpreter.hpp>

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <new>

namespace ShitHaneul {
#ifndef SHN_LITTLE
	Endian GetEndian() noexcept {
		static const std::uint32_t tester = 1;
		static const Endian endian = *reinterpret_cast<const std::uint8_t*>(&tester) == 1 ? Endian::Little : Endian::Big;
		return endian;
	}
#endif
}

namespace ShitHaneul {
	Page::Page(std::size_t count)
		: m_Page(count) {}
	Page::Page(Page&& page) noexcept
		: m_Page(std::move(page.m_Page)), m_Used(page.m_Used) {}

	Page& Page::operator=(Page&& page) noexcept {
		m_Page = std::move(page.m_Page);
		m_Used = page.m_Used;
		return *this;
	}
	const ManagedConstant& Page::operator[](std::size_t index) const noexcept {
		return m_Page[index];
	}
	ManagedConstant& Page::operator[](std::size_t index) noexcept {
		return m_Page[index];
	}

	bool Page::IsEmpty() const noexcept {
		return GetUnusedSize() == GetSize();
	}
	bool Page::IsFull() const noexcept {
		return GetUsedSize() == GetSize();
	}
	std::size_t Page::GetSize() const noexcept {
		return m_Page.size();
	}
	std::size_t Page::GetUsedSize() const noexcept {
		return m_Used;
	}
	std::size_t Page::GetUnusedSize() const noexcept {
		return GetSize() - GetUsedSize();
	}

	ManagedConstant* Page::Allocate() noexcept {
		if (IsFull()) return nullptr;
		else return &m_Page[m_Used++];
	}
}

namespace ShitHaneul {
	Generation::Generation(std::size_t pageSize) {
		m_Pages.emplace_back(pageSize);
		m_CurrentPage = m_Pages.begin();
		m_PageSize = pageSize;
	}
	Generation::Generation(Generation&& generation) noexcept
		: m_Pages(std::move(generation.m_Pages)), m_CurrentPage(generation.m_CurrentPage), m_PageSize(generation.m_PageSize) {}

	Generation& Generation::operator=(Generation&& generation) noexcept {
		m_Pages = std::move(generation.m_Pages);
		m_CurrentPage = generation.m_CurrentPage;
		m_PageSize = generation.m_PageSize;
		return *this;
	}

	Generation::PageIterator Generation::CreatePage() {
		const PageIterator result = m_Pages.insert(std::next(m_CurrentPage), Page(m_PageSize));
		return ++m_CurrentPage, result;
	}
	Generation::PageIterator Generation::GetCurrentPage() noexcept {
		return m_CurrentPage;
	}

	ManagedConstant* Generation::Allocate() noexcept {
		return m_CurrentPage->Allocate();
	}
}

namespace ShitHaneul {
	GarbageCollector::GarbageCollector(Interpreter& interpreter, std::size_t youngPageSize, std::size_t oldPageSize)
		: m_Interpreter(interpreter), m_FrontYoungGeneration(youngPageSize), m_BackYoungGeneration(youngPageSize),
		m_OldGeneration(oldPageSize) {}

	void GarbageCollector::Wait() {
		if (m_GCThread->joinable()) {
			m_GCThread->join();
		}
	}

	void GarbageCollector::Allocate(Function* reserved) noexcept {
		reserved->IsReady = true;
	}
	Function* GarbageCollector::Reserve() {
		ManagedConstant* const reserved = Reserve(false);
		*reserved = Function(false);
		return &std::get<Function>(*reserved);
	}

	ManagedConstant* GarbageCollector::Reserve(bool shouldDoMajorGC) {
		ManagedConstant* result = m_FrontYoungGeneration.Allocate();
		if (!result) {
			if (m_Status == Status::Idle) {
				std::swap(m_FrontYoungGeneration, m_BackYoungGeneration);
				StartGC(&GarbageCollector::MinorGC, true);
				if (!(result = m_FrontYoungGeneration.Allocate())) {
					result = CreatePageAndAllocate(false);
				}
			} else if (m_Status == Status::Done) {
				if (m_GCThread->joinable()) {
					m_GCThread->join();
				}
				std::swap(m_FrontYoungGeneration, m_BackYoungGeneration);
				if (!shouldDoMajorGC) {
					StartGC(&GarbageCollector::MinorGC, true);
				} else {
					m_Status = Status::Idle;
				}
				if (!(result = m_FrontYoungGeneration.Allocate())) {
					result = CreatePageAndAllocate(shouldDoMajorGC);
				}
			} else {
				result = CreatePageAndAllocate(shouldDoMajorGC);
			}
		}
		return result;
	}
	void GarbageCollector::StartGC(void(GarbageCollector::*pointer)(), bool shouldCreateNewThread) {
		m_Status = Status::Working;
		m_StackFrame = m_Interpreter.GetStackTrace();
		if (shouldCreateNewThread) {
			m_GCThread = std::make_unique<std::thread>(pointer, this);
		} else {
			(this->*pointer)();
		}
	}
	ManagedConstant* GarbageCollector::CreatePageAndAllocate(bool shouldDoMajorGC) {
		try {
			m_FrontYoungGeneration.CreatePage();
			return m_FrontYoungGeneration.Allocate();
		} catch (...) {
			if (shouldDoMajorGC) {
				StartGC(&GarbageCollector::MajorGC, false);
				return CreatePageAndAllocate(false);
			} else {
				if (m_GCThread->joinable()) {
					m_GCThread->join();
					return Reserve(true);
				} else return nullptr;
			}
		}
	}

	void GarbageCollector::MinorGC() {
		m_GCMaxGeneration = 0;

		// TODO
	}
	void GarbageCollector::MajorGC() {
		m_GCMaxGeneration = 1;

		// TODO
	}

	void GarbageCollector::Mark(const Constant& constant) {
		if (!constant.index()) return;

		const auto type = GetType(constant);
		if (type == Type::Function) {
			Mark(std::get<FunctionConstant>(constant).Value);
		} else if (type == Type::Structure) {
			Mark(std::get<StructureConstant>(constant).Value);
		}
	}
	void GarbageCollector::Mark(ManagedConstantRoot* constant) {
		if (constant->Generation > m_GCMaxGeneration || !constant->IsReady || constant->IsMarked) return;

		constant->IsMarked = true;
		m_GCPointerTable[constant];
		if (constant->Type == Type::Function) {
			const auto func = static_cast<Function*>(constant);
			const std::uint8_t josaCount = func->JosaMap.GetCount();
			for (std::uint8_t i = 0; i < josaCount; ++i) {
				Mark(func->JosaMap[i].second);
			}
			for (const Constant& freeVar : func->FreeVariableList) {
				Mark(freeVar);
			}
		} else if (constant->Type == Type::Structure) {
			const auto structure = static_cast<Structure*>(constant);
			const std::uint8_t fieldCount = structure->GetCount();
			for (std::uint8_t i = 0; i < fieldCount; ++i) {
				Mark((*structure)[i].second);
			}
		}
	}
}