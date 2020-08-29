#pragma once

#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Josa.hpp>

#include <cstdint>
#include <string>

namespace ShitHaneul {
	class FunctionInfo final {
	public:
		std::string Name;
		std::string Path;
		std::uint16_t Line;

		std::uint64_t StackOperandCount;
		std::uint32_t LocalVariableCount;
		ShitHaneul::ConstantList ConstantList;

		ShitHaneul::JosaMap JosaMap;

		// TODO: GlobalList, LineTable, Instructions

	public:
		FunctionInfo() noexcept = default;
		FunctionInfo(FunctionInfo&& functionInfo) noexcept;
		~FunctionInfo() = default;

	public:
		FunctionInfo& operator=(FunctionInfo&& functionInfo) noexcept;
	};
}