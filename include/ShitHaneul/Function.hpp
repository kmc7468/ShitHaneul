#pragma once

#include <ShitHaneul/ConstantList.hpp>

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

namespace ShitHaneul {
	class FunctionInfo final {
	public:
		std::string Name;
		std::string Path;
		std::uint16_t Line;

		std::uint64_t StackOperandCount;
		std::uint32_t LocalVariableCount;
		ShitHaneul::ConstantList ConstantList;

		// TODO: JosaList, GlobalList, LineTable, Instructions

	public:
		FunctionInfo() noexcept = default;
		FunctionInfo(FunctionInfo&& functionInfo) noexcept;
		~FunctionInfo() = default;

	public:
		FunctionInfo& operator=(FunctionInfo&& functionInfo) noexcept;
	};
}