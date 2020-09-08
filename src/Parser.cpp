#include <ShitHaneul/Parser.hpp>

#include <ShitHaneul/Interpreter.hpp>

#include <algorithm>
#include <fstream>
#include <ios>
#include <memory>
#include <utility>

namespace ShitHaneul {
	ByteFile::ByteFile(ByteFile&& byteFile) noexcept
		: m_GlobalNameMap(std::move(byteFile.m_GlobalNameMap)),
		m_FunctionInfos(std::move(byteFile.m_FunctionInfos)), m_Functions(std::move(byteFile.m_Functions)),
		m_RootFunction(byteFile.m_RootFunction),
		m_StructureNameMap(std::move(byteFile.m_StructureNameMap)), m_StructureInfos(std::move(byteFile.m_StructureInfos)),
		m_Structures(std::move(byteFile.m_Structures)) {}
	ByteFile::~ByteFile() {
		Clear();
	}

	ByteFile& ByteFile::operator=(ByteFile&& byteFile) noexcept {
		m_GlobalNameMap = std::move(byteFile.m_GlobalNameMap);

		m_FunctionInfos = std::move(byteFile.m_FunctionInfos);
		m_Functions = std::move(byteFile.m_Functions);
		m_RootFunction = byteFile.m_RootFunction;

		m_StructureNameMap = std::move(byteFile.m_StructureNameMap);
		m_StructureInfos = std::move(byteFile.m_StructureInfos);
		m_Structures = std::move(byteFile.m_Structures);
		return *this;
	}

	void ByteFile::Clear() noexcept {
		m_GlobalNameMap.clear();
		m_StructureNameMap.clear();

		static constexpr auto deleter = [](auto pointer) {
			delete pointer;
		};

		std::for_each(m_FunctionInfos.begin(), m_FunctionInfos.end(), deleter);
		std::for_each(m_Functions.begin(), m_Functions.end(), deleter);
		std::for_each(m_Structures.begin(), m_Structures.end(), deleter);
	}

	std::size_t ByteFile::GetGlobalNameCount() const noexcept {
		return m_GlobalNameMap.size();
	}
	std::size_t ByteFile::GetGlobalNameIndex(const std::u32string& name, bool createNewIndex) {
		std::size_t& index = m_GlobalNameMap[name];
		if (createNewIndex && !index) {
			index = m_GlobalNameMap.size();
		}
		return index;
	}

	Function* ByteFile::RegisterFunction(FunctionInfo* functionInfo) {
		m_FunctionInfos.push_back(functionInfo);

		std::unique_ptr<Function> result(new Function(functionInfo));
		AddFunction(result.get());
		return result.release();
	}
	void ByteFile::AddFunction(Function* function) {
		m_Functions.push_back(function);
	}
	Function* ByteFile::CopyFunction(const Function* function) {
		std::unique_ptr<Function> result(new Function(*function));
		AddFunction(result.get());
		return result.release();
	}
	const Function* ByteFile::GetRoot() const noexcept {
		return m_RootFunction;
	}
	Function* ByteFile::GetRoot() noexcept {
		return m_RootFunction;
	}
	void ByteFile::SetRoot(Function* function) noexcept {
		m_RootFunction = function;
	}

	std::size_t ByteFile::GetStructureNameCount() const noexcept {
		return m_StructureNameMap.size();
	}
	std::size_t ByteFile::GetStructureNameIndex(const std::u32string& name) {
		std::size_t& index = m_StructureNameMap[name];
		if (!index) {
			index = m_StructureNameMap.size();
		}
		return index;
	}
	void ByteFile::RegisterStructure(std::size_t index, const StringList& structureInfo) {
		m_StructureInfos[index] = structureInfo;
	}
	void ByteFile::AllocateStructures(std::size_t required) {
		m_Structures.reserve(m_Structures.size() + required);
	}
	void ByteFile::AllocateStructureInfos() {
		m_StructureInfos.resize(GetStructureNameCount() + 1);
	}
	void ByteFile::AddStructure(StringMap* structure) {
		m_Structures.push_back(structure);
	}
	StringMap* ByteFile::CreateStructure(std::size_t index) {
		std::unique_ptr<StringMap> result(new StringMap(m_StructureInfos[index]));
		AddStructure(result.get());
		return result.release();
	}
	const StringList& ByteFile::GetStructureInfo(std::size_t index) {
		return m_StructureInfos[index];
	}
}

namespace ShitHaneul {
	bool Parser::Load(const char* path) {
		std::ifstream stream(path, std::ifstream::binary);
		if (!stream) return false;

		stream.seekg(0, std::ifstream::end);
		const std::streamsize length = static_cast<std::streamsize>(stream.tellg());
		stream.seekg(0, std::ifstream::beg);

		m_Bytes.resize(static_cast<std::size_t>(length));
		stream.read(reinterpret_cast<char*>(m_Bytes.data()), length);
		if (stream.gcount() != length) return false;

		m_Cursor = 0;
		m_Result.Clear();
		return true;
	}
	void Parser::Parse() {
		m_Result.SetRoot(ParseFunction());
	}
	ByteFile Parser::GetResult() noexcept {
		return std::move(m_Result);
	}

	char32_t Parser::ReadCharacter() {
		const auto first = ReadScalar<std::uint8_t>();
		if (first < 0x80) return EncodeUTF8ToUTF32(first, 0, 0, 0);
		else if (first < 0xE0) {
			const auto second = ReadScalar<std::uint8_t>();
			return EncodeUTF8ToUTF32(static_cast<char>(first), static_cast<char>(second), 0, 0);
		} else if (first < 0xF0) {
			const auto second = ReadScalar<std::uint8_t>();
			const auto third = ReadScalar<std::uint8_t>();
			return EncodeUTF8ToUTF32(static_cast<char>(first), static_cast<char>(second), static_cast<char>(third), 0);
		} else {
			const auto second = ReadScalar<std::uint8_t>();
			const auto third = ReadScalar<std::uint8_t>();
			const auto fourth = ReadScalar<std::uint8_t>();
			return EncodeUTF8ToUTF32(static_cast<char>(first), static_cast<char>(second), static_cast<char>(third), static_cast<char>(fourth));
		}
	}

	Function* Parser::ParseFunction() {
		std::unique_ptr<FunctionInfo> info(new FunctionInfo);
		info->JosaList = ParseStringList();
		info->GlobalList = ParseGlobalList();
		info->StackOperandCount = ReadScalar<std::uint64_t>();
		info->LocalVariableCount = ReadScalar<std::uint32_t>();
		info->ConstantList = ParseConstantList();
		info->Name = ReadString();
		info->Line.Path = ReadString();
		info->Line.Line = ReadScalar<std::uint16_t>();
		info->LineMap = ParseLineMap(info->Line);
		info->InstructionList = ParseInstructionList();

		Function* const result = m_Result.RegisterFunction(info.get());
		return info.release(), result;
	}
	StringList Parser::ParseStringList() {
		StringList result;

		const auto count = ReadScalar<std::uint8_t>();
		result.Reserve(count);
		for (std::uint8_t i = 0; i < count; ++i) {
			result.Add(ReadString<std::uint8_t>());
		}
		return result;
	}
	std::vector<std::size_t> Parser::ParseGlobalList() {
		std::vector<std::size_t> result;

		const auto count = ReadScalar<std::uint64_t>();
		result.reserve(static_cast<std::size_t>(count));
		for (std::uint64_t i = 0; i < count; ++i) {
			result.push_back(m_Result.GetGlobalNameIndex(ReadString()));
		}
		return result;
	}
	ConstantList Parser::ParseConstantList() {
		ConstantList result;

		const auto count = ReadScalar<std::uint64_t>();
		result.Reserve(count);
		for (std::uint64_t i = 0; i < count; ++i) {
			const auto type = ReadScalar<Type>();
			switch (type) {
			case Type::None:
				result.Add();
				break;

			case Type::Integer:
				result.Add(ReadScalar<std::int64_t>());
				break;

			case Type::Real:
				result.Add(ReadScalar<double>());
				break;

			case Type::Character:
				result.Add(ReadCharacter());
				break;

			case Type::Boolean:
				result.Add(ReadScalar<bool>());
				break;

			case Type::Function:
				result.Add(ParseFunction());
				break;
			}
		}
		return result;
	}
	LineMap Parser::ParseLineMap(const LineInfo& line) {
		LineMap result;

		const auto count = ReadScalar<std::uint64_t>();
		result.Reserve(count + 2);
		result.Add(0, line.Line, std::u32string(line.Path));

		for (std::uint64_t i = 0; i < count; ++i) {
			const auto offset = ReadScalar<std::uint32_t>();
			const auto isLine = ReadScalar<bool>();
			if (isLine) {
				result.Add(offset, ReadScalar<std::uint16_t>());
			} else {
				result.Add(offset, ReadString());
			}
		}
		return result;
	}
	InstructionList Parser::ParseInstructionList() {
		InstructionList result;

		const auto count = ReadScalar<std::uint64_t>();
		result.Reserve(count);
		for (std::uint64_t i = 0; i < count; ++i) {
			result.Add(ParseInstruction());
		}
		return result;
	}
	Instruction Parser::ParseInstruction() {
		Instruction result;

		result.OpCode = ReadScalar<OpCode>();
		switch (result.OpCode) {
		case OpCode::Push:
		case OpCode::LoadLocal:
		case OpCode::StoreLocal:
		case OpCode::LoadDeref:
		case OpCode::StoreGlobal:
		case OpCode::LoadGlobal:
		case OpCode::PopJmpIfFalse:
		case OpCode::Jmp:
			result.Operand = ReadScalar<std::uint32_t>();
			break;

		case OpCode::FreeVar:
			result.Operand = ParseFreeVariableList();
			break;

		case OpCode::Call:
			result.Operand = ParseStringList();
			break;

		case OpCode::AddStruct:
		case OpCode::MakeStruct: {
			const auto name = ReadString<std::uint8_t>();
			result.Operand = std::make_pair(m_Result.GetStructureNameIndex(name), ParseStringList());
			break;
		}

		case OpCode::GetField:
			result.Operand = ReadString<std::uint8_t>();
			break;
		}
		return result;
	}
	FreeVariableList Parser::ParseFreeVariableList() {
		FreeVariableList result;

		const auto count = ReadScalar<std::uint8_t>();
		result.reserve(static_cast<std::size_t>(count));
		for (std::uint8_t i = 0; i < count; ++i) {
			const auto variableType = ReadScalar<VariableType>();
			const auto index = ReadScalar<std::uint8_t>();
			result.push_back({ variableType, index });
		}
		return result;
	}
}