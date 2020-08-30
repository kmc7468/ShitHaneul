#pragma once

#include <ShitHaneul/ByteFile.hpp>
#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Function.hpp>
#include <ShitHaneul/Instruction.hpp>
#include <ShitHaneul/Josa.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace ShitHaneul {
	class Parser final {
	private:
		std::vector<std::uint8_t> m_Bytes;
		std::size_t m_Cursor = 0;

		ByteFile m_Result;

	public:
		Parser() noexcept = default;
		Parser(const Parser&) = delete;
		~Parser() = default;

	public:
		Parser& operator=(const Parser&) = delete;

	public:
		void Load(const std::string& path);
		void Parse();
		ByteFile GetResult() noexcept;

	private:
		template<typename T>
		T ReadScalar();
		char32_t ReadCharacter();
		template<typename S = std::uint64_t>
		std::u32string ReadString();

		Function* ParseFunction();
		JosaList ParseJosaList();
		std::vector<std::pair<std::size_t, std::u32string>> ParseGlobalList();
		ConstantList ParseConstantList();
		LineMap ParseLineMap();
		InstructionList ParseInstructionList();
		Instruction ParseInstruction();
		FreeVariableList ParseFreeVariableList();
	};
}

#include "impl/Parser.hpp"