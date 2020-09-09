#include <ShitHaneul/Memory.hpp>

#include <utility>

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