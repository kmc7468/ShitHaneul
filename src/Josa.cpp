#include <ShitHaneul/Josa.hpp>

#include <algorithm>
#include <cassert>
#include <functional>

namespace ShitHaneul {
	JosaMap::JosaMap(const JosaMap& josaMap) noexcept
		: m_Map(josaMap.m_Map) {}
	JosaMap::JosaMap(JosaMap&& josaMap) noexcept
		: m_Map(std::move(josaMap.m_Map)) {}

	JosaMap& JosaMap::operator=(const JosaMap& josaMap) noexcept {
		m_Map = josaMap.m_Map;
		return *this;
	}
	JosaMap& JosaMap::operator=(JosaMap&& josaMap) noexcept {
		m_Map = std::move(josaMap.m_Map);
		return *this;
	}
	const Josa& JosaMap::operator[](std::uint8_t index) const noexcept {
		return m_Map[static_cast<std::size_t>(index)].first;
	}

	void JosaMap::AddJosa(std::string&& josa) {
		const std::size_t hash = std::hash<std::string>{}(josa);
		m_Map.push_back({ { std::move(josa), hash }, {} });
	}
	std::uint8_t JosaMap::GetCount() const noexcept {
		return static_cast<std::uint8_t>(m_Map.size());
	}
	void JosaMap::ReserveJosas(std::uint8_t count) {
		m_Map.reserve(static_cast<std::size_t>(count));
	}
	void JosaMap::BindConstant(const std::string_view& josa, const Constant& constant) {
		const std::size_t hash = std::hash<std::string_view>{}(josa);
		const auto iter = std::find_if(m_Map.begin(), m_Map.end(), [hash](const auto& element) {
			return element.first.Hash == hash;
		});
		iter->second = constant;
	}
}