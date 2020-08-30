#pragma once

#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Instruction.hpp>
#include <ShitHaneul/Josa.hpp>

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace ShitHaneul {
	struct LineInfo final {
		std::uint64_t Offset = UINT64_MAX;
		std::uint16_t Line = UINT16_MAX;
		std::string Path;
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
		void Add(std::uint64_t offset, std::string&& path);
		std::uint64_t GetCount() const noexcept;
		void Reserve(std::uint64_t count);
	};
}

namespace ShitHaneul {
	class FunctionInfo final {
	public:
		std::string Name;
		LineInfo Line;

		std::uint64_t StackOperandCount = 0;
		std::uint32_t LocalVariableCount = 0;
		ShitHaneul::ConstantList ConstantList;
		std::vector<Constant> GlobalList;

		ShitHaneul::JosaMap JosaMap;
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