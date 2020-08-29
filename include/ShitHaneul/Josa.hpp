#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace ShitHaneul {
	struct Josa final {
		std::string String;
		std::size_t Hash;
	};

	class JosaList final {
	private:
		std::vector<Josa> m_List;

	public:
		JosaList() noexcept = default;
		JosaList(JosaList&& josaList) noexcept;
		~JosaList() = default;

	public:
		JosaList& operator=(JosaList&& josaList) noexcept;
		const Josa& operator[](std::uint8_t index) const noexcept;

	public:
		void AddJosa(std::string&& josa);
		std::uint8_t GetCount() const noexcept;
		void ReserveJosas(std::uint8_t count);
	};
}