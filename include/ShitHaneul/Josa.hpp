#pragma once

#include <ShitHaneul/Constant.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace ShitHaneul {
	class JosaList final {
	private:
		std::vector<std::pair<std::size_t, std::string>> m_List;

	public:
		JosaList() noexcept = default;
		JosaList(JosaList&& josaList) noexcept;
		~JosaList() = default;

	public:
		JosaList& operator=(JosaList&& josaList) noexcept;
		const std::pair<std::size_t, std::string>& operator[](std::uint8_t index) const noexcept;

	public:
		void Add(std::string&& josa);
		std::uint8_t GetCount() const noexcept;
		void Reserve(std::uint8_t count);
	};
}

namespace ShitHaneul {
	class JosaMap final {
	private:
		std::vector<std::pair<std::pair<std::size_t, std::string_view>, Constant>> m_Map;

	public:
		explicit JosaMap(const JosaList& josaList);
		JosaMap(const JosaMap& josaMap) noexcept;
		JosaMap(JosaMap&& josaMap) noexcept;
		~JosaMap() = default;

	public:
		JosaMap& operator=(const JosaMap& josaMap) noexcept;
		JosaMap& operator=(JosaMap&& josaMap) noexcept;

	public:
		void BindConstant(const std::string_view& josa, const Constant& constant);
	};
}