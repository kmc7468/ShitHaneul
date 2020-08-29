#include <ShitHaneul/Josa.hpp>

#include <algorithm>
#include <cassert>
#include <functional>

namespace ShitHaneul {
	JosaMap::JosaMap(JosaMap&& josaList) noexcept
		: m_List(std::move(josaList.m_List)) {}

	JosaMap& JosaMap::operator=(JosaMap&& josaList) noexcept {
		m_List = std::move(josaList.m_List);
		return *this;
	}
	const Josa& JosaMap::operator[](std::uint8_t index) const noexcept {
		return m_List[static_cast<std::size_t>(index)].first;
	}

	void JosaMap::AddJosa(std::string&& josa) {
		const std::size_t hash = std::hash<std::string>{}(josa);
		m_List.push_back({ { std::move(josa), hash }, {} });
	}
	std::uint8_t JosaMap::GetCount() const noexcept {
		return static_cast<std::uint8_t>(m_List.size());
	}
	void JosaMap::ReserveJosas(std::uint8_t count) {
		m_List.reserve(static_cast<std::size_t>(count));
	}
	void JosaMap::BindConstant(const std::string_view& josa, const Constant& constant) {
		const std::size_t hash = std::hash<std::string_view>{}(josa);
		const auto iter = std::find_if(m_List.begin(), m_List.end(), [hash](const auto& element) {
			return element.first.Hash == hash;
		});
		iter->second = constant;
	}
}