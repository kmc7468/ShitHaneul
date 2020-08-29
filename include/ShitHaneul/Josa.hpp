#pragma once

#include <ShitHaneul/Constant.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace ShitHaneul {
	struct Josa final {
		std::string String;
		std::size_t Hash;
	};
}

namespace ShitHaneul {
	class JosaMap;

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

		JosaMap MakeMap() const;
	};
}

namespace ShitHaneul {
	class JosaMap final {
	private:
		std::vector<std::pair<Josa, Constant>> m_Map;

	public:

	};
}