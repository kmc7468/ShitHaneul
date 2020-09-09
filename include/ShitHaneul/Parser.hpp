#pragma once

#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Function.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace ShitHaneul {
	class ByteFile final {
	private:
		std::unordered_map<std::u32string, std::size_t> m_GlobalNameMap;

		std::vector<FunctionInfo*> m_FunctionInfos;
		std::vector<Function*> m_Functions;
		Function* m_RootFunction = nullptr;

		std::unordered_map<std::u32string, std::size_t> m_StructureNameMap;
		std::vector<StringList> m_StructureInfos;
		std::vector<StringMap*> m_Structures;

	public:
		ByteFile() = default;
		ByteFile(ByteFile&& byteFile) noexcept;
		~ByteFile();

	public:
		ByteFile& operator=(ByteFile&& byteFile) noexcept;

	public:
		void Clear() noexcept;

		std::size_t GetGlobalNameCount() const noexcept;
		std::size_t GetGlobalNameIndex(const std::u32string& name, bool createNewIndex = true);

		Function* RegisterFunction(FunctionInfo* functionInfo);
		void AddFunction(Function* function);
		Function* CopyFunction(const Function* function);
		const Function* GetRoot() const noexcept;
		Function* GetRoot() noexcept;
		void SetRoot(Function* function) noexcept;

		std::size_t GetStructureNameCount() const noexcept;
		std::size_t GetStructureNameIndex(const std::u32string& name);
		void RegisterStructure(std::size_t index, const StringList& structureInfo);
		void AllocateStructures(std::size_t required);
		void AllocateStructureInfos();
		void AddStructure(StringMap* structure);
		StringMap* CreateStructure(std::size_t index);
		const StringList& GetStructureInfo(std::size_t index);
	};
}

namespace ShitHaneul {
	class Parser final {
	private:
		std::vector<std::uint8_t> m_Bytes;
		std::size_t m_Cursor = 0;

		ByteFile m_Result;

	public:
		Parser() = default;
		Parser(const Parser&) = delete;
		~Parser() = default;

	public:
		Parser& operator=(const Parser&) = delete;

	public:
		bool Load(const char* path);
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
		std::vector<std::size_t> ParseGlobalList();
		ConstantList ParseConstantList();
		LineMap ParseLineMap(const LineInfo& line);
		InstructionList ParseInstructionList();
		Instruction ParseInstruction();
		FreeVariableList ParseFreeVariableList();
	};
}

#include "impl/Parser.hpp"