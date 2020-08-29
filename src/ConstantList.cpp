#include <ShitHaneul/ConstantList.hpp>

#include <cstddef>
#include <utility>

namespace ShitHaneul {
	ConstantList::ConstantList(ConstantList&& constantList) noexcept
		: m_List(std::move(constantList.m_List)) {}

	ConstantList& ConstantList::operator=(ConstantList&& constantList) noexcept {
		m_List = std::move(constantList.m_List);
		return *this;
	}
	Constant ConstantList::operator[](std::uint64_t index) const noexcept {
		return m_List[static_cast<std::size_t>(index)];
	}

	std::uint64_t ConstantList::GetCount() const noexcept {
		return static_cast<std::uint64_t>(m_List.size());
	}
	void ConstantList::ReserveConstants(std::uint64_t count) {
		m_List.reserve(static_cast<std::size_t>(count));
	}
}