#include <ShitHaneul/Instruction.hpp>

namespace ShitHaneul {
	Instruction::Instruction(Instruction&& instruction) noexcept
		: OpCode(instruction.OpCode), Operand(std::move(instruction.Operand)) {}

	Instruction& Instruction::operator=(Instruction&& instruction) noexcept {
		OpCode = instruction.OpCode;
		Operand = std::move(instruction.Operand);
		return *this;
	}

	std::string_view Instruction::GetMnemonic() const noexcept {
		return Mnemonic[static_cast<std::uint8_t>(OpCode)];
	}
}