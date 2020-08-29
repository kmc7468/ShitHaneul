#pragma once

#include <ShitHaneul/Constant.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace ShitHaneul {
	struct Josa final {
		std::string String;
		std::size_t Hash;
	};
}

namespace ShitHaneul {
	class JosaMap final {
	private:
		std::vector<std::pair<Josa, Constant>> m_Map;

	public:
		JosaMap() noexcept = default;
		JosaMap(const JosaMap& josaMap) noexcept;
		JosaMap(JosaMap&& josaMap) noexcept;
		~JosaMap() = default;

	public:
		JosaMap& operator=(const JosaMap& josaMap) noexcept;
		JosaMap& operator=(JosaMap&& josaMap) noexcept;
		const Josa& operator[](std::uint8_t index) const noexcept;

	public:
		void AddJosa(std::string&& josa);
		std::uint8_t GetCount() const noexcept;
		void ReserveJosas(std::uint8_t count);
		void BindConstant(const std::string_view& josa, const Constant& constant);
	};
}