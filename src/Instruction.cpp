#include <ShitHaneul/Instruction.hpp>

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