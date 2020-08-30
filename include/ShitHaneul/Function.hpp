#pragma once

#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Instruction.hpp>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace ShitHaneul {
	struct LineInfo final {
		std::uint64_t Offset = UINT64_MAX;
		std::uint16_t Line = UINT16_MAX;
		std::u32string Path;
	};

	class LineMap final {
	private:
		std::vector<LineInfo> m_Map;

	public:
		LineMap() noexcept = default;
		LineMap(LineMap&& lineMap) noexcept;
		~LineMap() = default;

	public:
		LineMap& operator=(LineMap&& lineMap) noexcept;
		const LineInfo& operator[](std::uint64_t index) const noexcept;

	public:
		void Add(std::uint64_t offset, std::uint16_t line);
		void Add(std::uint64_t offset, std::u32string&& path);
		std::uint64_t GetCount() const noexcept;
		void Reserve(std::uint64_t count);
	};
}

namespace ShitHaneul {
	class FunctionInfo final {
	public:
		std::u32string Name;
		LineInfo Line;

		std::uint64_t StackOperandCount = 0;
		std::uint32_t LocalVariableCount = 0;
		ShitHaneul::ConstantList ConstantList;
		std::vector<std::pair<std::size_t, std::u32string>> GlobalList;

		StringList JosaList;
		ShitHaneul::LineMap LineMap;
		ShitHaneul::InstructionList InstructionList;

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
		StringMap JosaMap;
		std::vector<Constant> FreeVariableList;

	public:
		Function() noexcept = default;
		explicit Function(const FunctionInfo* info);
		Function(const Function& function);
		Function(Function&& function) noexcept;
		~Function() = default;

	public:
		Function& operator=(const Function& function) noexcept;
		Function& operator=(Function&& function) noexcept;
	};
}