#pragma once

#include <ShitHaneul/Constant.hpp>

#include <cstdint>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace ShitHaneul {
	enum class OpCode : std::uint8_t {
		Push,
		Pop,
		LoadLocal,
		StoreLocal,
		LoadDeref,
		StoreGlobal,
		LoadGlobal,

		Call,
		AddStruct,
		MakeStruct,
		GetField,
		Jmp,
		PopJmpIfFalse,
		FreeVar,

		Add,
		Subtract,
		Multiply,
		Divide,
		Mod,

		Equal,
		LessThan,
		GreaterThan,
		Negate,
		LogicNot,
		LogicAnd,
		LogicOr,
	};
}

namespace ShitHaneul {
	enum class VariableType : std::uint8_t {
		Local,
		Free,
	};

	using FreeVariableList = std::vector<std::pair<VariableType, std::uint8_t>>;
}

namespace ShitHaneul {
	using Operand = std::variant<std::monostate,
		std::uint32_t, std::u32string,
		FreeVariableList, StringList,
		std::pair<std::u32string, StringList>>;

	class Instruction final {
	public:
		ShitHaneul::OpCode OpCode;
		ShitHaneul::Operand Operand;

	public:
		Instruction() noexcept = default;
		Instruction(Instruction&& instruction) noexcept;
		~Instruction() = default;

	public:
		Instruction& operator=(Instruction&& instruction) noexcept;
	};

	class InstructionList final {
	private:
		std::vector<Instruction> m_List;

	public:
		InstructionList() noexcept = default;
		InstructionList(InstructionList&& instructionList) noexcept;
		~InstructionList() = default;

	public:
		InstructionList& operator=(InstructionList&& instructionList) noexcept;
		const Instruction& operator[](std::uint64_t index) const noexcept;

	public:
		void Add(Instruction&& instruction);
		std::uint64_t GetCount() const noexcept;
		void Reserve(std::uint64_t count);
	};
}