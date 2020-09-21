#include <ShitHaneul/Function.hpp>

#include <ShitHaneul/Interpreter.hpp>

namespace ShitHaneul {
	Instruction::Instruction(Instruction&& instruction) noexcept
		: OpCode(instruction.OpCode), Operand(std::move(instruction.Operand)) {}

	Instruction& Instruction::operator=(Instruction&& instruction) noexcept {
		OpCode = instruction.OpCode;
		Operand = std::move(instruction.Operand);
		return *this;
	}
}

namespace ShitHaneul {
	InstructionList::InstructionList(InstructionList&& instructionList) noexcept
		: m_List(std::move(instructionList.m_List)) {}

	InstructionList& InstructionList::operator=(InstructionList&& instructionList) noexcept {
		m_List = std::move(instructionList.m_List);
		return *this;
	}
	const Instruction& InstructionList::operator[](std::uint64_t index) const noexcept {
		return m_List[static_cast<std::size_t>(index)];
	}

	void InstructionList::Add(Instruction&& instruction) {
		m_List.push_back(std::move(instruction));
	}
	std::uint64_t InstructionList::GetCount() const noexcept {
		return static_cast<std::uint64_t>(m_List.size());
	}
	void InstructionList::Reserve(std::uint64_t count) {
		m_List.reserve(static_cast<std::size_t>(count));
	}
}

namespace ShitHaneul {
	LineMap::LineMap(LineMap&& lineMap) noexcept
		: m_Map(std::move(lineMap.m_Map)) {}

	LineMap& LineMap::operator=(LineMap&& lineMap) noexcept {
		m_Map = std::move(lineMap.m_Map);
		return *this;
	}
	LineInfo LineMap::operator[](std::uint64_t offset) const {
		auto line = m_Map.begin(), path = line;
		for (auto iter = line + 1; iter < m_Map.end(); ++iter) {
			if (offset < iter->Offset) break;
			else if (iter->Path.empty()) {
				line = iter;
			} else {
				path = iter;
			}
		}
		return { offset, line->Line, path->Path };
	}

	void LineMap::Add(std::uint64_t offset, std::uint16_t line) {
		m_Map.push_back({ offset, line });
	}
	void LineMap::Add(std::uint64_t offset, std::u32string&& path) {
		m_Map.push_back({ offset, UINT16_MAX, std::move(path) });
	}
	void LineMap::Add(std::uint64_t offset, std::uint16_t line, std::u32string&& path) {
		m_Map.push_back({ offset, line, path });
	}
	std::uint64_t LineMap::GetCount() const noexcept {
		return static_cast<std::uint64_t>(m_Map.size());
	}
	void LineMap::Reserve(std::uint64_t count) {
		m_Map.reserve(static_cast<std::size_t>(count));
	}
}

namespace ShitHaneul {
	FunctionInfo::FunctionInfo(StringList&& josaList, std::function<Constant(std::uint64_t, const StringMap&)>&& builtinFunction)
		: JosaList(std::move(josaList)), BuiltinFunction(std::move(builtinFunction)) {}
	FunctionInfo::FunctionInfo(FunctionInfo&& functionInfo) noexcept
		: Name(std::move(functionInfo.Name)), Line(std::move(functionInfo.Line)),
		StackOperandCount(functionInfo.StackOperandCount), LocalVariableCount(functionInfo.LocalVariableCount),
		ConstantList(std::move(functionInfo.ConstantList)), GlobalList(std::move(functionInfo.GlobalList)),
		JosaList(std::move(functionInfo.JosaList)), LineMap(std::move(functionInfo.LineMap)),
		BuiltinFunction(std::move(functionInfo.BuiltinFunction)), InstructionList(std::move(functionInfo.InstructionList)),
		m_RecycledStackFrames(std::move(functionInfo.m_RecycledStackFrames)) {}

	FunctionInfo& FunctionInfo::operator=(FunctionInfo&& functionInfo) noexcept {
		Name = std::move(functionInfo.Name);
		Line = std::move(functionInfo.Line);

		StackOperandCount = functionInfo.StackOperandCount;
		LocalVariableCount = functionInfo.LocalVariableCount;
		ConstantList = std::move(functionInfo.ConstantList);
		GlobalList = std::move(functionInfo.GlobalList);

		JosaList = std::move(functionInfo.JosaList);
		LineMap = std::move(functionInfo.LineMap);
		BuiltinFunction = std::move(functionInfo.BuiltinFunction);
		InstructionList = std::move(functionInfo.InstructionList);

		m_RecycledStackFrames = std::move(functionInfo.m_RecycledStackFrames);
		return *this;
	}

	std::size_t FunctionInfo::GetStackSize() const noexcept {
		return GetStackStartOffset() + static_cast<std::size_t>(StackOperandCount);
	}
	std::size_t FunctionInfo::GetStackStartOffset() const noexcept {
		return static_cast<std::size_t>(LocalVariableCount + JosaList.GetCount());
	}

	bool FunctionInfo::IsRecyclable() const noexcept {
		return !m_RecycledStackFrames.empty();
	}
	StackFrame FunctionInfo::Recycle() noexcept {
		StackFrame result = std::move(m_RecycledStackFrames.back());
		m_RecycledStackFrames.erase(m_RecycledStackFrames.end() - 1);
		return std::move(result);
	}
	void FunctionInfo::Recycle(StackFrame&& stackFrame) {
		m_RecycledStackFrames.push_back(std::move(stackFrame));
	}
}

namespace ShitHaneul {
	Function::Function() noexcept
		: ManagedConstantHeader(Type::Function) {}
	Function::Function(FunctionInfo* info)
		: ManagedConstantHeader(Type::Function), Info(info), JosaMap(info->JosaList) {}
	Function::Function(const Function& function)
		: ManagedConstantHeader(Type::Function), Info(function.Info), JosaMap(function.JosaMap), FreeVariableList(function.FreeVariableList) {}
	Function::Function(Function&& function) noexcept
		: ManagedConstantHeader(Type::Function), Info(function.Info), JosaMap(std::move(function.JosaMap)), FreeVariableList(std::move(function.FreeVariableList)) {}

	Function& Function::operator=(const Function& function) noexcept {
		Info = function.Info;
		JosaMap = function.JosaMap;
		FreeVariableList = function.FreeVariableList;
		return *this;
	}
	Function& Function::operator=(Function&& function) noexcept {
		Info = function.Info;
		JosaMap = std::move(function.JosaMap);
		FreeVariableList = std::move(function.FreeVariableList);
		return *this;
	}
}