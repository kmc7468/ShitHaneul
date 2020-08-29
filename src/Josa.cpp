#include <ShitHaneul/Josa.hpp>

#include <functional>
#include <utility>

namespace ShitHaneul {
	JosaList::JosaList(JosaList&& josaList) noexcept
		: m_List(std::move(josaList.m_List)) {}

	JosaList& JosaList::operator=(JosaList&& josaList) noexcept {
		m_List = std::move(josaList.m_List);
		return *this;
	}
	const Josa& JosaList::operator[](std::uint8_t index) const noexcept {
		return m_List[static_cast<std::size_t>(index)];
	}

	void JosaList::AddJosa(std::string&& josa) {
		const std::size_t hash = std::hash<std::string>{}(josa);
		m_List.push_back({ std::move(josa), hash });
	}
	std::uint8_t JosaList::GetCount() const noexcept {
		return static_cast<std::uint8_t>(m_List.size());
	}
	void JosaList::ReserveJosas(std::uint8_t count) {
		m_List.reserve(static_cast<std::size_t>(count));
	}
}