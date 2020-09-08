#pragma once

#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Function.hpp>
#include <ShitHaneul/Parser.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ShitHaneul {
	class StackFrame final {
	private:
		std::vector<Constant> m_Stack;
		std::size_t m_Top;

		Function* m_CurrentFunction;
		std::uint64_t m_CurrentOffset = 0;

	public:
		StackFrame(Function* currentFunction, StackFrame* prevStackFrame);
		StackFrame(StackFrame&& stackFrame) noexcept;
		~StackFrame() = default;

	public:
		StackFrame& operator=(StackFrame&& stackFrame) noexcept;

	public:
		void Recycle(Function* currentFunction, StackFrame* prevStackFrame, bool resetLocal = true) noexcept;

		void Push(const Constant& constant) noexcept;
		void Pop() noexcept;
		Constant& GetTop() noexcept;
		Constant GetTopAndPop() noexcept;
		Constant& GetUnderTop() noexcept;

		void Store(std::uint32_t index);
		void StoreDirect(std::uint32_t index, const Constant& constant);
		void Load(std::uint32_t index);
		Constant& LoadDirect(std::uint32_t index);

		const Function* GetCurrentFunction() const noexcept;
		Function* GetCurrentFunction() noexcept;
		std::uint64_t GetCurrentOffset() const noexcept;
		void SetCurrentOffset(std::uint64_t newCurrentOffset) noexcept;
	};
}

namespace ShitHaneul {
	struct Exception final {
		const FunctionInfo* Function;
		LineInfo Line;
		std::string Mesasge;
	};
}

namespace ShitHaneul {
	class Interpreter final {
	private:
		ByteFile m_ByteFile;
		Exception m_Exception;

		std::vector<StackFrame> m_StackTrace;
		std::vector<Constant> m_GlobalVariables;

	public:
		Interpreter() = default;
		Interpreter(const Interpreter&) = delete;
		~Interpreter() = default;

	public:
		Interpreter& operator=(const Interpreter&) = delete;

	public:
		void Load(ByteFile&& byteFile);
		bool Interpret();
		const Exception& GetException() const noexcept;
		const std::vector<StackFrame>& GetStackTrace() const noexcept;

	private:
		void RegisterBuiltinFunction(const std::u32string& name, StringList&& josaList,
			std::function<Constant(std::uint64_t, const StringMap&)>&& builtinFunction);
		void RegisterBuiltinFunctions();
		Constant ConvertStringToList(const std::u32string& string);
		std::optional<std::u32string> ConvertListToString(std::uint64_t offset, const Constant& list);

		void RaiseException(std::uint64_t offset, std::string&& message);
		static std::string InvalidTypeException(const std::string_view& expected, const std::string_view& given);
		static std::string UndefinedException(const std::string_view& type, const std::string_view& name);
		static std::string UndefinedFunctionException();
		static std::string NoJosaException(const std::string_view& name);
		static std::string AlreadyBoundException(const std::string_view& type, const std::string_view& name);
		static std::string FieldMismatchException(std::uint8_t expected, std::uint8_t given);
		static std::string DivideByZeroException();
		static std::string UnaryTypeException(const std::string_view& type, const std::string_view& operation);
		static std::string BinaryTypeException(const std::string_view& lhs, const std::string_view& rhs, const std::string_view& operation);
	};
}

namespace ShitHaneul {
	char32_t EncodeUTF8ToUTF32(char first, char second, char third, char fourth) noexcept;
	std::u32string EncodeUTF8ToUTF32(const std::string_view& utf8);
	std::string EncodeUTF32ToUTF8(char32_t character);
	std::string EncodeUTF32ToUTF8(const std::u32string_view& utf32);

	char32_t ReadCharacterFromStdin();
	void WriteCharacterToStdout(char32_t character);
	void WriteStringToStdout(const std::string_view& string);
}