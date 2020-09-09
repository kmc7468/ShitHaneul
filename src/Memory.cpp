#include <ShitHaneul/Memory.hpp>

#include <iterator>

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

	void Generation::Reset() noexcept {
		m_Pages.clear();
	}
	void Generation::Initialize(std::size_t pageSize) {
		m_Pages.emplace_back(pageSize);
		m_CurrentPage = m_Pages.begin();
		m_PageSize = pageSize;
	}

	Generation::PageIterator Generation::CreatePage() {
		return m_Pages.insert(std::next(m_CurrentPage), Page(m_PageSize));
	}
	Generation::PageIterator Generation::GetCurrentPage() noexcept {
		return m_CurrentPage;
	}
}