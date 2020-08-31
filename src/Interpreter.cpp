#include <ShitHaneul/Interpreter.hpp>

#include <utility>

namespace ShitHaneul {
	StackFrame::StackFrame(Function* currentFunction)
		: m_Top(static_cast<std::size_t>(currentFunction->Info->LocalVariableCount)), m_CurrentFunction(currentFunction) {}
	StackFrame::StackFrame(StackFrame&& stackFrame) noexcept
		: m_Stack(std::move(stackFrame.m_Stack)), m_Top(stackFrame.m_Top),
		m_CurrentFunction(stackFrame.m_CurrentFunction), m_CurrentOffset(stackFrame.m_CurrentOffset) {}

	StackFrame& StackFrame::operator=(StackFrame&& stackFrame) noexcept {
		m_Stack = std::move(stackFrame.m_Stack);
		m_Top = stackFrame.m_Top;

		m_CurrentFunction = stackFrame.m_CurrentFunction;
		m_CurrentOffset = stackFrame.m_CurrentOffset;
		return *this;
	}

	void StackFrame::Push(const Constant& constant) noexcept {
		m_Stack[m_Top++] = constant;
	}
	void StackFrame::Pop() noexcept {
		--m_Top;
	}
	const Constant& StackFrame::GetTop() const noexcept {
		return m_Stack[m_Top - 1];
	}

	void StackFrame::Store(std::uint32_t index) {
		m_Stack[static_cast<std::size_t>(index)] = m_Stack[--m_Top];
	}
	void StackFrame::Load(std::uint32_t index) {
		m_Stack[m_Top++] = m_Stack[static_cast<std::size_t>(index)];
	}

	const Function* StackFrame::GetCurrentFunction() const noexcept {
		return m_CurrentFunction;
	}
	Function* StackFrame::GetCurrentFunction() noexcept {
		return m_CurrentFunction;
	}
	std::uint64_t StackFrame::GetCurrentOffset() const noexcept {
		return m_CurrentOffset;
	}
	void StackFrame::SetCurrentOffset(std::uint64_t newCurrentOffset) noexcept {
		m_CurrentOffset = newCurrentOffset;
	}
}

namespace ShitHaneul {
	void Interpreter::Load(ByteFile&& byteFile) {
		m_ByteFile = std::move(byteFile);

		m_StackTrace.clear();
		m_StackTrace.emplace_back(m_ByteFile.GetRoot());
		m_GlobalVariables.clear();
		m_Structures.clear();
	}
	bool Interpreter::Interpret() {
#define frame m_StackTrace.back()
#define func frame.GetCurrentFunction()
#define funcInfo func->Info
#define intOperand std::get<std::uint32_t>(instruction.Operand)
#define strOperand std::get<std::u32string>(instruction.Operand)
#define strListOperand std::get<StringList>(instruction.Operand)
#define typeName(type) TypeName[static_cast<std::uint8_t>(type)]

		for (std::uint64_t offset = 0; offset < funcInfo->InstructionList.GetCount(); ++offset) {
			const Instruction& instruction = funcInfo->InstructionList[offset];
			switch (instruction.OpCode) {
			case OpCode::Push:
				frame.Push(funcInfo->ConstantList[intOperand]);
				break;

			case OpCode::Pop:
				frame.Pop();
				break;

			case OpCode::LoadLocal:
				frame.Load(intOperand);
				break;

			case OpCode::StoreLocal:
				frame.Store(intOperand);
				break;

			case OpCode::LoadDeref:
				frame.Push(func->FreeVariableList[static_cast<std::size_t>(intOperand)]);
				break;

			case OpCode::StoreGlobal:
				m_GlobalVariables[funcInfo->GlobalList[static_cast<std::size_t>(intOperand)]] = frame.GetTop();
				frame.Pop();
				break;

			case OpCode::LoadGlobal:
				frame.Push(m_GlobalVariables[funcInfo->GlobalList[static_cast<std::size_t>(intOperand)]]);
				break;
			}
		}
		return true;
	}
	const Exception& Interpreter::GetException() const noexcept {
		return m_Exception;
	}
	const std::vector<StackFrame>& Interpreter::GetStackTrace() const noexcept {
		return m_StackTrace;
	}

	void Interpreter::RaiseException(std::uint64_t offset, std::string&& message) {
		m_Exception.Function = funcInfo;
		m_Exception.Line = funcInfo->LineMap[offset];
		m_Exception.Mesasge = std::move(message);

		frame.SetCurrentOffset(offset);
	}
	std::string Interpreter::InvalidTypeException(const std::string_view& expected, const std::string_view& given) {
		std::string result(expected);
		result += u8" 자료형의 값을 받아야하는데 ";
		result += given;
		result += u8" 자료형의 값이 주어졌습니다.";
		return result;
	}
	std::string Interpreter::UnboundException(const std::string_view& type, const std::string_view& name) {
		std::string result(type);
		result += u8" '";
		result += name;
		result += u8"'을/를 찾을 수 없습니다.";
		return result;
	}
	std::string Interpreter::UndefinedFunctionException() {
		return u8"선언은 되었으나 정의되지 않은 함수를 호출할 수 없습니다.";
	}
	std::string Interpreter::FieldMismatchException(std::uint8_t expected, std::uint8_t given) {
		std::string result = u8"구조체에 ";
		result += std::to_string(expected);
		result += u8"개의 필드가 있는데 ";
		result += std::to_string(given);
		result += u8"개의 값이 주어졌습니다.";
		return result;
	}
	std::string Interpreter::DivideByZeroException() {
		return u8"0으로 나눌 수 없습니다.";
	}
	std::string Interpreter::UnaryTypeException(const std::string_view& type, const std::string_view& operation) {
		std::string result(type);
		result += u8" 자료형의 값의 ";
		result += operation;
		result += u8" 연산은 지원되지 않습니다.";
		return result;
	}
	std::string Interpreter::BinaryTypeException(const std::string_view& lhs, const std::string_view& rhs, const std::string_view& operation) {
		std::string result(lhs);
		result += u8" 자료형의 값과 ";
		result += rhs;
		result += u8" 자료형의 값끼리의 ";
		result += operation;
		result += u8" 연산은 지원되지 않습니다.";
		return result;
	}
}