#include <ShitHaneul/Memory.hpp>

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
		return &m_Page[m_Used++];
	}
}

namespace ShitHaneul {
	Generation::Generation(std::size_t pageSize) {
		Initialize(pageSize);
	}
	Generation::Generation(Generation&& generation) noexcept
		: m_Pages(std::move(generation.m_Pages)), m_CurrentPage(generation.m_CurrentPage), m_PageSize(generation.m_PageSize) {}

	Generation& Generation::operator=(Generation&& generation) noexcept {
		m_Pages = std::move(generation.m_Pages);
		m_CurrentPage = generation.m_CurrentPage;
		m_PageSize = generation.m_PageSize;
		return *this;
	}

	bool Generation::IsEmpty() const noexcept {
		return m_Pages.size() == 0;
	}
	void Generation::Reset() noexcept {
		m_Pages.clear();
	}
	void Generation::Initialize(std::size_t pageSize) {
		m_Pages.emplace_back(pageSize);
		m_CurrentPage = m_Pages.begin();
		m_PageSize = pageSize;
	}

	Generation::PageIterator Generation::CreatePage() {
		const PageIterator result = m_Pages.insert(std::next(m_CurrentPage), Page(m_PageSize));
		return ++m_CurrentPage, result;
	}
	Generation::PageIterator Generation::GetCurrentPage() noexcept {
		return m_CurrentPage;
	}

	ManagedConstant* Generation::Allocate() noexcept {
		if (m_CurrentPage->IsFull()) return nullptr;
		else return m_CurrentPage->Allocate();
	}
}

namespace ShitHaneul {
	GarbageCollector::GarbageCollector(std::size_t youngPageSize, std::size_t oldPageSize)
		: m_FrontYoungGeneration(youngPageSize), m_FrontOldGeneration(oldPageSize) {}
	GarbageCollector::GarbageCollector(GarbageCollector&& garbageCollector) noexcept
		: m_FrontYoungGeneration(std::move(garbageCollector.m_FrontYoungGeneration)), m_BackYoungGeneration(std::move(garbageCollector.m_BackYoungGeneration)),
		m_FrontOldGeneration(std::move(garbageCollector.m_FrontOldGeneration)), m_BackOldGeneration(std::move(garbageCollector.m_BackOldGeneration)) {
		assert(m_Status == Status::Idle && garbageCollector.m_Status == Status::Idle);
	}

	GarbageCollector& GarbageCollector::operator=(GarbageCollector&& garbageCollector) noexcept {
		assert(m_Status == Status::Idle && garbageCollector.m_Status == Status::Idle);

		m_FrontYoungGeneration = std::move(garbageCollector.m_FrontYoungGeneration);
		m_BackYoungGeneration = std::move(garbageCollector.m_BackYoungGeneration);
		m_FrontOldGeneration = std::move(garbageCollector.m_FrontOldGeneration);
		m_BackOldGeneration = std::move(garbageCollector.m_BackOldGeneration);
		return *this;
	}

	void GarbageCollector::Reset() noexcept {
		m_FrontYoungGeneration.Reset();
		m_BackYoungGeneration.Reset();
		m_FrontOldGeneration.Reset();
		m_BackOldGeneration.Reset();
	}
	void GarbageCollector::Initialize(std::size_t youngPageSize, std::size_t oldPageSize) {
		m_FrontYoungGeneration.Initialize(youngPageSize);
		m_FrontOldGeneration.Initialize(oldPageSize);
	}

	ManagedConstant* GarbageCollector::Allocate(Interpreter& interpreter) {
		return Allocate(interpreter, false);
	}

	ManagedConstant* GarbageCollector::Allocate(Interpreter& interpreter, bool shouldDoMajorGC) {
		ManagedConstant* result = m_FrontYoungGeneration.Allocate();
		if (!result) {
			if (m_Status == Status::Idle) {
				SwapFrontAndBack(true, false);
				StartGC(&GarbageCollector::MinorGC, true, interpreter);
				if (!(result = m_FrontYoungGeneration.Allocate())) {
					result = CreatePageAndAllocate(interpreter, false);
				}
			} else if (m_Status == Status::Done) {
				if (m_GCThread->joinable()) {
					m_GCThread->join();
				}
				SwapFrontAndBack(m_ShouldSwapYoungGeneration, m_ShouldSwapOldGeneration);
				if (!shouldDoMajorGC) {
					StartGC(&GarbageCollector::MinorGC, true, interpreter);
				} else {
					m_Status = Status::Idle;
				}
				if (!(result = m_FrontYoungGeneration.Allocate())) {
					result = CreatePageAndAllocate(interpreter, shouldDoMajorGC);
				}
			} else {
				result = CreatePageAndAllocate(interpreter, shouldDoMajorGC);
			}
		}
		return result;
	}
	void GarbageCollector::SwapFrontAndBack(bool shouldSwapYoungGeneration, bool shouldSwapOldGeneration) noexcept {
		if (shouldSwapYoungGeneration) {
			std::swap(m_FrontYoungGeneration, m_BackYoungGeneration);
		}
		if (shouldSwapOldGeneration) {
			std::swap(m_FrontOldGeneration, m_BackOldGeneration);
		}
	}
	void GarbageCollector::StartGC(void(GarbageCollector::*pointer)(Interpreter&), bool shouldCreateNewThread, Interpreter& interpreter) {
		m_Status = Status::Working;
		if (shouldCreateNewThread) {
			m_GCThread = std::make_unique<std::thread>(pointer, this, std::ref(interpreter));
		} else {
			(this->*pointer)(interpreter);
		}
	}
	ManagedConstant* GarbageCollector::CreatePageAndAllocate(Interpreter& interpreter, bool shouldDoMajorGC) {
		try {
			m_FrontYoungGeneration.CreatePage();
			return m_FrontYoungGeneration.Allocate();
		} catch (...) {
			if (shouldDoMajorGC) {
				MajorGC(interpreter);
				return CreatePageAndAllocate(interpreter, false);
			} else {
				if (m_GCThread->joinable()) {
					m_GCThread->join();
					return Allocate(interpreter, true);
				} else return nullptr;
			}
		}
	}

	void GarbageCollector::MinorGC(Interpreter& interpreter) {
		// TODO
	}
	void GarbageCollector::MajorGC(Interpreter& interpreter) {
		// TODO
	}
}