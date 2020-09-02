#pragma once

#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Function.hpp>
#include <ShitHaneul/Parser.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace ShitHaneul {
	class StackFrame final {
	private:
		std::vector<Constant> m_Stack;
		std::size_t m_Top;

		Function* m_CurrentFunction;
		std::uint64_t m_CurrentOffset = 0;

	public:
		StackFrame(Function* currentFunction);
		StackFrame(StackFrame&& stackFrame) noexcept;
		~StackFrame() = default;

	public:
		StackFrame& operator=(StackFrame&& stackFrame) noexcept;

	public:
		void Push(const Constant& constant) noexcept;
		void Pop() noexcept;
		const Constant& GetTop() const noexcept;

		void Store(std::uint32_t index);
		void Load(std::uint32_t index);

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
		std::unordered_map<std::u32string, Constant> m_GlobalVariables;
		std::unordered_map<std::u32string, StringList> m_Structures;

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
		void RaiseException(std::uint64_t offset, std::string&& message);
		static std::string InvalidTypeException(const std::string_view& expected, const std::string_view& given);
		static std::string UndefinedException(const std::string_view& type, const std::string_view& name);
		static std::string NoJosaException(const std::string_view& name);
		static std::string AlreadyBoundException(const std::string_view& type, const std::string_view& name);
		static std::string FieldMismatchException(std::uint8_t expected, std::uint8_t given);
		static std::string DivideByZeroException();
		static std::string UnaryTypeException(const std::string_view& type, const std::string_view& operation);
		static std::string BinaryTypeException(const std::string_view& lhs, const std::string_view& rhs, const std::string_view& operation);
	};
}