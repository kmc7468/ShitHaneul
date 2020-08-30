#include <ShitHaneul/Josa.hpp>

#include <algorithm>
#include <functional>

namespace ShitHaneul {
	JosaList::JosaList(JosaList&& josaList) noexcept
		: m_List(std::move(josaList.m_List)) {}

	JosaList& JosaList::operator=(JosaList&& josaList) noexcept {
		m_List = std::move(josaList.m_List);
		return *this;
	}
	const std::pair<std::size_t, std::string>& JosaList::operator[](std::uint8_t index) const noexcept {
		return m_List[static_cast<std::size_t>(index)];
	}

	void JosaList::Add(std::string&& josa) {
		const std::size_t hash = std::hash<std::string>{}(josa);
		m_List.push_back({ hash, std::move(josa) });
	}
	std::uint8_t JosaList::GetCount() const noexcept {
		return static_cast<std::uint8_t>(m_List.size());
	}
	void JosaList::Reserve(std::uint8_t count) {
		m_List.reserve(static_cast<std::size_t>(count));
	}
}

namespace ShitHaneul {
	JosaMap::JosaMap(const JosaList& josaList) {
		const std::uint8_t count = josaList.GetCount();
		m_Map.reserve(static_cast<std::size_t>(count));

		for (std::uint8_t i = 0; i < count; ++i) {
			m_Map.push_back({ josaList[i], {} });
		}
	}
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

	void JosaMap::BindConstant(const std::string_view& josa, const Constant& constant) {
		const std::size_t hash = std::hash<std::string_view>{}(josa);
		const auto iter = std::find_if(m_Map.begin(), m_Map.end(), [josa, hash](const auto& element) {
			return element.first.first == hash && element.first.second == josa;
		});
		iter->second = constant;
	}
}