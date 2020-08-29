#pragma once

#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Josa.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace ShitHaneul {
	class FunctionInfo final {
	public:
		std::string Name;
		std::string Path;
		std::uint16_t Line = 0;

		std::uint64_t StackOperandCount = 0;
		std::uint32_t LocalVariableCount = 0;
		ShitHaneul::ConstantList ConstantList;
		std::vector<Constant> GlobalList;

		ShitHaneul::JosaMap JosaMap;

		// TODO: LineTable, Instructions

	public:
		FunctionInfo() noexcept = default;
		FunctionInfo(FunctionInfo&& functionInfo) noexcept;
		~FunctionInfo() = default;

	public:
		FunctionInfo& operator=(FunctionInfo&& functionInfo) noexcept;
	};
}

namespace ShitHaneul {
	class Function final {
	public:
		const FunctionInfo* Info = nullptr;
		ShitHaneul::JosaMap JosaMap;
		std::vector<Constant> FreeVariableList;

	public:
		Function() noexcept = default;
		explicit Function(const FunctionInfo* info);
		Function(Function&& function) noexcept;
		~Function() = default;

	public:
		Function& operator=(Function&& function) noexcept;
	};
}