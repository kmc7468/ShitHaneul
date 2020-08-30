#pragma once

#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Function.hpp>

#include <cstddef>
#include <cstdint>
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