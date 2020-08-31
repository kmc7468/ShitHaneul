#pragma once

#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Function.hpp>
#include <ShitHaneul/Parser.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace ShitHaneul {
	class StackFrame final {
	private:
		std::vector<Constant> m_Stack;
		std::size_t m_Top;

		const FunctionInfo* m_CurrentFunction;
		std::uint64_t m_Offset = 0;

	public:
		StackFrame(const FunctionInfo* currentFunction);
		StackFrame(StackFrame&& stackFrame) noexcept;
		~StackFrame() = default;

	public:
		StackFrame& operator=(StackFrame&& stackFrame) noexcept;

	public:
		void Push(const Constant& constant);
		Constant Pop();
		const Constant& GetTop() const noexcept;

		void Store(std::uint32_t index);
		void Load(std::uint32_t index);
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
		const Exception& GetException() const noexcept;
		const std::vector<StackFrame>& GetStackTrace() const noexcept;
	};
}