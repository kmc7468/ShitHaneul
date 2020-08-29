#pragma once

#include <cstdint>
#include <string_view>

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

	static constexpr std::string_view Mnemonic[] = {
		"PUSH", "POP", "LOAD_LOCAL", "STORE_LOCAL", "LOAD_DEREF", "STORE_GLOBAL", "LOAD_GLOBAL",
		"CALL", "ADD_STRUCT", "MAKE_STRUCT", "GET_FIELD", "JMP", "POP_JMP_IF_FALSE", "FREE_VAR",
		"ADD", "SUBTRACT", "MULTIPLY", "DIVIDE", "MOD",
		"EQUAL", "LESS_THAN", "GREATER_THAN", "NEGATE", "LOGIC_NOT", "LOGIC_AND", "LOGIC_OR"
	};
}