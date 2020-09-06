#pragma once

#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Instruction.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
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
		LineInfo operator[](std::uint64_t offset) const;

	public:
		void Add(std::uint64_t offset, std::uint16_t line);
		void Add(std::uint64_t offset, std::u32string&& path);
		void Add(std::uint64_t offset, std::uint16_t line, std::u32string&& path);
		std::uint64_t GetCount() const noexcept;
		void Reserve(std::uint64_t count);
	};
}

namespace ShitHaneul {
	class StackFrame;

	class FunctionInfo final {
	public:
		std::u32string Name;
		LineInfo Line;

		std::uint64_t StackOperandCount = 0;
		std::uint32_t LocalVariableCount = 0;
		ShitHaneul::ConstantList ConstantList;
		std::vector<std::size_t> GlobalList;

		StringList JosaList;
		ShitHaneul::LineMap LineMap;
		std::function<Constant(std::uint64_t, const StringMap&)> BuiltinFunction;
		ShitHaneul::InstructionList InstructionList;

	private:
		std::vector<StackFrame> m_RecycledStackFrames;

	public:
		FunctionInfo() noexcept = default;
		FunctionInfo(StringList&& josaList, std::function<Constant(std::uint64_t, const StringMap&)>&& builtinFunction);
		FunctionInfo(FunctionInfo&& functionInfo) noexcept;
		~FunctionInfo() = default;

	public:
		FunctionInfo& operator=(FunctionInfo&& functionInfo) noexcept;

	public:
		std::size_t GetStackSize() const noexcept;
		std::size_t GetStackStartOffset() const noexcept;

		bool IsRecyclable() const noexcept;
		StackFrame Recycle() noexcept;
		void Recycle(StackFrame&& stackFrame);
	};
}

namespace ShitHaneul {
	class Function final {
	public:
		FunctionInfo* Info = nullptr;
		StringMap JosaMap;
		std::vector<Constant> FreeVariableList;

	public:
		Function() noexcept = default;
		explicit Function(FunctionInfo* info);
		Function(const Function& function);
		Function(Function&& function) noexcept;
		~Function() = default;

	public:
		Function& operator=(const Function& function) noexcept;
		Function& operator=(Function&& function) noexcept;
	};
}