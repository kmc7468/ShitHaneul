#pragma once

#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Function.hpp>
#include <ShitHaneul/Instruction.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace ShitHaneul {
	class ByteFile final {
	private:
		std::vector<FunctionInfo*> m_FunctionInfos;
		std::vector<Function*> m_Functions;
		Function* m_RootFunction = nullptr;

	public:
		ByteFile() noexcept = default;
		ByteFile(ByteFile&& byteFile) noexcept;
		~ByteFile();

	public:
		ByteFile& operator=(ByteFile&& byteFile) noexcept;

	public:
		void Clear() noexcept;

		Function* RegisterFunction(FunctionInfo* functionInfo);
		void AddFunction(Function* function);
		void SetRoot(Function* function);
	};
}

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
		StringList ParseStringList();
		std::vector<std::pair<std::size_t, std::u32string>> ParseGlobalList();
		ConstantList ParseConstantList();
		LineMap ParseLineMap();
		InstructionList ParseInstructionList();
		Instruction ParseInstruction();
		FreeVariableList ParseFreeVariableList();
	};
}

#include "impl/Parser.hpp"