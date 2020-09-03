#include <ShitHaneul/Interpreter.hpp>

#include <memory>
#include <utility>
#include <variant>

namespace ShitHaneul {
	StackFrame::StackFrame(Function* currentFunction)
		: m_Stack(static_cast<std::size_t>(currentFunction->Info->LocalVariableCount + currentFunction->Info->StackOperandCount)),
		m_Top(static_cast<std::size_t>(currentFunction->Info->LocalVariableCount)), m_CurrentFunction(currentFunction) {
		const std::uint8_t argCount = currentFunction->JosaMap.GetCount();
		for (std::uint8_t i = 0; i < argCount; ++i) {
			m_Stack[static_cast<std::size_t>(i)] = currentFunction->JosaMap[i];
		}
	}
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
	Constant& StackFrame::GetTop() noexcept {
		return m_Stack[m_Top - 1];
	}
	Constant& StackFrame::GetUnderTop() noexcept {
		return m_Stack[m_Top - 2];
	}

	void StackFrame::Store(std::uint32_t index) {
		StoreDirect(index, m_Stack[--m_Top]);
	}
	void StackFrame::StoreDirect(std::uint32_t index, const Constant& constant) {
		Constant& variable = m_Stack[static_cast<std::size_t>(index)];
		if (GetType(constant) == Type::Function && std::get<FunctionConstant>(constant).IsForwardDeclared) {
			const Function* const func = std::get<FunctionConstant>(constant).Value;
			Function* const target = std::get<FunctionConstant>(variable).Value;
			target->FreeVariableList = func->FreeVariableList;
			target->Info = func->Info;
			target->JosaMap = func->JosaMap;
		} else {
			variable = constant;
		}
	}
	void StackFrame::Load(std::uint32_t index) {
		m_Stack[m_Top++] = LoadDirect(index);
	}
	Constant& StackFrame::LoadDirect(std::uint32_t index) {
		return m_Stack[static_cast<std::size_t>(index)];
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

namespace {
	template<typename... F>
	struct Overload : F... {
		using F::operator()...;
	};
	template<typename... F>
	Overload(F...) -> Overload<F...>;
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
#define freeVarListOperand std::get<FreeVariableList>(instruction.Operand)
#define typeName(type) TypeName[static_cast<std::uint8_t>(type)]

		std::uint64_t offset = 0;
	start:
		for (; offset < funcInfo->InstructionList.GetCount(); ++offset) {
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

			case OpCode::Call: {
				if (const auto type = GetType(frame.GetTop()); type != Type::Function) {
					RaiseException(offset, InvalidTypeException(u8"함수", typeName(type)));
					return false;
				}

				const auto target = std::get<FunctionConstant>(frame.GetTop());
				Function* const newFunc = m_ByteFile.CopyFunction(target.Value);
				for (std::uint8_t i = 0; i < strListOperand.GetCount(); ++i) {
					if (strListOperand[i].second == U"_") {
						const BoundResult result = newFunc->JosaMap.BindConstant(frame.GetTop());
						if (result != BoundResult::Success) {
							RaiseException(offset,
								result == BoundResult::Undefiend ? NoJosaException(/*TODO*/"") :
								AlreadyBoundException(u8"함수", /*TODO*/""));
							return false;
						}
					} else {
						const BoundResult result = newFunc->JosaMap.BindConstant(strListOperand[i].second, frame.GetTop());
						if (result != BoundResult::Success) {
							RaiseException(offset,
								result == BoundResult::Undefiend ? UndefinedException(u8"조사", /*TODO*/"") :
								AlreadyBoundException(u8"조사", /*TODO*/""));
							return false;
						}
					}
					frame.Pop();
				}

				if (newFunc->JosaMap.GetUnboundCount()) {
					frame.Push(FunctionConstant(newFunc));
				} else {
					frame.SetCurrentOffset(offset);
					offset = static_cast<std::uint64_t>(-1);
					m_StackTrace.emplace_back(newFunc);
				}
				break;
			}

			case OpCode::AddStruct:
				m_Structures[strOperand] = strListOperand;
				break;

			case OpCode::MakeStruct: {
				const StringList& fields = m_Structures[strOperand];
				const std::uint8_t expectedFieldCount = fields.GetCount();
				const std::uint8_t givenFieldCount = strListOperand.GetCount();
				if (expectedFieldCount != givenFieldCount) {
					RaiseException(offset, FieldMismatchException(expectedFieldCount, givenFieldCount));
					return false;
				}

				std::unique_ptr<StringMap> structure(new StringMap(fields));
				for (std::uint8_t i = 0; i < givenFieldCount; ++i) {
					if (!fields.Contains(strListOperand[i].second)) {
						RaiseException(offset, UndefinedException(u8"필드", /*TODO*/""));
						return false;
					}
					structure->BindConstant(frame.GetTop());
					frame.Pop();
				}

				frame.Push(StructureConstant(structure.get()));
				m_ByteFile.AddStructure(structure.get());
				structure.release();
				break;
			}

			case OpCode::GetField: {
				if (const auto type = GetType(frame.GetTop()); type != Type::Structure) {
					RaiseException(offset, InvalidTypeException(u8"구조체", typeName(type)));
					return false;
				}

				const auto target = std::get<StructureConstant>(frame.GetTop());
				frame.Pop();
				frame.Push((*target.Value)[strOperand]);
				break;
			}

			case OpCode::Jmp:
				offset = intOperand - 1;
				break;

			case OpCode::PopJmpIfFalse: {
				if (const auto type = GetType(frame.GetTop()); type != Type::Boolean) {
					RaiseException(offset, InvalidTypeException(u8"부울", typeName(type)));
					return false;
				}

				const auto target = std::get<BooleanConstant>(frame.GetTop());
				if (!target.Value) {
					offset = intOperand - 1;
				}
				frame.Pop();
				break;
			}

			case OpCode::FreeVar: {
				auto& target = std::get<FunctionConstant>(frame.GetTop());
				const auto count = static_cast<std::uint8_t>(freeVarListOperand.size());
				for (std::uint8_t i = 0; i < count; ++i) {
					const auto [type, index] = freeVarListOperand[i];
					if (type == VariableType::Local) {
						target.Value->FreeVariableList.push_back(frame.LoadDirect(index));
					} else {
						target.Value->FreeVariableList.push_back(frame.GetCurrentFunction()->FreeVariableList[static_cast<std::size_t>(index)]);
					}
				}
				break;
			}

			case OpCode::Add: {
				Constant& top = frame.GetTop();
				Constant& underTop = frame.GetUnderTop();
				const auto topType = GetType(top), underTopType = GetType(underTop);
				if (underTopType == Type::Integer) {
					if (topType == Type::Integer) {
						frame.Push(IntegerConstant(std::get<IntegerConstant>(underTop).Value + std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Push(RealConstant(std::get<IntegerConstant>(underTop).Value + std::get<RealConstant>(top).Value));
					} else goto addError;
				} else if (underTopType == Type::Real) {
					if (topType == Type::Integer) {
						frame.Push(RealConstant(std::get<RealConstant>(underTop).Value + std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Push(RealConstant(std::get<RealConstant>(underTop).Value + std::get<RealConstant>(top).Value));
					} else goto addError;
				} else {
				addError:
					RaiseException(offset, BinaryTypeException(typeName(underTopType), typeName(topType), u8"더하기"));
					return false;
				}
				break;
			}

			case OpCode::Subtract: {
				Constant& top = frame.GetTop();
				Constant& underTop = frame.GetUnderTop();
				const auto topType = GetType(top), underTopType = GetType(underTop);
				if (underTopType == Type::Integer) {
					if (topType == Type::Integer) {
						frame.Push(IntegerConstant(std::get<IntegerConstant>(underTop).Value - std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Push(RealConstant(std::get<IntegerConstant>(underTop).Value - std::get<RealConstant>(top).Value));
					} else goto subError;
				} else if (underTopType == Type::Real) {
					if (topType == Type::Integer) {
						frame.Push(RealConstant(std::get<RealConstant>(underTop).Value - std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Push(RealConstant(std::get<RealConstant>(underTop).Value - std::get<RealConstant>(top).Value));
					} else goto subError;
				} else {
				subError:
					RaiseException(offset, BinaryTypeException(typeName(underTopType), typeName(topType), u8"빼기"));
					return false;
				}
				break;
			}

			case OpCode::Multiply: {
				Constant& top = frame.GetTop();
				Constant& underTop = frame.GetUnderTop();
				const auto topType = GetType(top), underTopType = GetType(underTop);
				if (underTopType == Type::Integer) {
					if (topType == Type::Integer) {
						frame.Push(IntegerConstant(std::get<IntegerConstant>(underTop).Value * std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Push(RealConstant(std::get<IntegerConstant>(underTop).Value * std::get<RealConstant>(top).Value));
					} else goto mulError;
				} else if (underTopType == Type::Real) {
					if (topType == Type::Integer) {
						frame.Push(RealConstant(std::get<RealConstant>(underTop).Value * std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Push(RealConstant(std::get<RealConstant>(underTop).Value * std::get<RealConstant>(top).Value));
					} else goto mulError;
				} else {
				mulError:
					RaiseException(offset, BinaryTypeException(typeName(underTopType), typeName(topType), u8"곱하기"));
					return false;
				}
				break;
			}

			case OpCode::Divide: {
				Constant& top = frame.GetTop();
				Constant& underTop = frame.GetUnderTop();
				const auto topType = GetType(top), underTopType = GetType(underTop);
				if (underTopType == Type::Integer) {
					if (topType == Type::Integer) {
						if (std::get<IntegerConstant>(top).Value == 0) goto divZeroError;
						frame.Push(IntegerConstant(std::get<IntegerConstant>(underTop).Value / std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						if (std::get<RealConstant>(top).Value == 0) goto divZeroError;
						frame.Push(RealConstant(std::get<IntegerConstant>(underTop).Value / std::get<RealConstant>(top).Value));
					} else goto divError;
				} else if (underTopType == Type::Real) {
					if (topType == Type::Integer) {
						if (std::get<IntegerConstant>(top).Value == 0) goto divZeroError;
						frame.Push(RealConstant(std::get<RealConstant>(underTop).Value / std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						if (std::get<RealConstant>(top).Value == 0) {
						divZeroError:
							RaiseException(offset, DivideByZeroException());
							return false;
						}
						frame.Push(RealConstant(std::get<RealConstant>(underTop).Value / std::get<RealConstant>(top).Value));
					} else goto divError;
				} else {
				divError:
					RaiseException(offset, BinaryTypeException(typeName(underTopType), typeName(topType), u8"나누기"));
					return false;
				}
				break;
			}

			case OpCode::Mod: {
				Constant& top = frame.GetTop();
				Constant& underTop = frame.GetUnderTop();
				if (const auto topType = GetType(top), underTopType = GetType(underTop); topType != underTopType || topType != Type::Integer) {
					RaiseException(offset, BinaryTypeException(typeName(underTopType), typeName(topType), u8"나머지"));
					return false;
				}

				if (std::get<IntegerConstant>(top).Value == 0) {
					RaiseException(offset, DivideByZeroException());
					return false;
				}
				frame.Push(IntegerConstant(std::get<IntegerConstant>(underTop).Value % std::get<IntegerConstant>(top).Value));
				break;
			}

			case OpCode::Equal: {
				Constant& top = frame.GetTop();
				Constant& underTop = frame.GetUnderTop();
				const auto topType = GetType(top), underTopType = GetType(underTop);
				if (underTopType == Type::Function) {
					RaiseException(offset, BinaryTypeException(typeName(underTopType), typeName(topType), u8"비교"));
					return false;
				} else {
					frame.Push(BooleanConstant(Equal(underTop, top)));
				}
				break;
			}

			case OpCode::LessThan : {
				Constant& top = frame.GetTop();
				Constant& underTop = frame.GetUnderTop();
				const auto topType = GetType(top), underTopType = GetType(underTop);
				if (underTopType == Type::Integer) {
					if (topType == Type::Integer) {
						frame.Push(BooleanConstant(std::get<IntegerConstant>(underTop).Value < std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Push(BooleanConstant(std::get<IntegerConstant>(underTop).Value < std::get<RealConstant>(top).Value));
					} else goto lessThanError;
				} else if (underTopType == Type::Real) {
					if (topType == Type::Integer) {
						frame.Push(BooleanConstant(std::get<RealConstant>(underTop).Value < std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Push(BooleanConstant(std::get<RealConstant>(underTop).Value < std::get<RealConstant>(top).Value));
					} else goto lessThanError;
				} else {
				lessThanError:
					RaiseException(offset, BinaryTypeException(typeName(underTopType), typeName(topType), u8"대소 비교"));
					return false;
				}
				break;
			}

			case OpCode::GreaterThan: {
				Constant& top = frame.GetTop();
				Constant& underTop = frame.GetUnderTop();
				const auto topType = GetType(top), underTopType = GetType(underTop);
				if (underTopType == Type::Integer) {
					if (topType == Type::Integer) {
						frame.Push(BooleanConstant(std::get<IntegerConstant>(underTop).Value > std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Push(BooleanConstant(std::get<IntegerConstant>(underTop).Value > std::get<RealConstant>(top).Value));
					} else goto greaterThanError;
				} else if (underTopType == Type::Real) {
					if (topType == Type::Integer) {
						frame.Push(BooleanConstant(std::get<RealConstant>(underTop).Value > std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Push(BooleanConstant(std::get<RealConstant>(underTop).Value > std::get<RealConstant>(top).Value));
					} else goto greaterThanError;
				} else {
				greaterThanError:
					RaiseException(offset, BinaryTypeException(typeName(underTopType), typeName(topType), u8"대소 비교"));
					return false;
				}
				break;
			}

			case OpCode::Negate: {
				Constant& top = frame.GetTop();
				if (!std::visit(Overload{
					[](IntegerConstant& constant) {
						constant.Value = -constant.Value;
						return true;
					},
					[](RealConstant& constant) {
						constant.Value = -constant.Value;
						return true;
					},
					[&](auto& constant) {
						RaiseException(offset, UnaryTypeException(typeName(GetType(constant)), u8"부호 반전"));
						return false;
					}
				}, top)) return false;
				break;
			}

			case OpCode::LogicNot: {
				Constant& top = frame.GetTop();
				if (const auto type = GetType(top); type != Type::Boolean) {
					RaiseException(offset, UnaryTypeException(typeName(type), u8"논리 부정"));
					return false;
				}

				auto& operand = std::get<BooleanConstant>(top);
				operand.Value = !operand.Value;
				break;
			}

			case OpCode::LogicAnd: {
				Constant& top = frame.GetTop();
				Constant& underTop = frame.GetUnderTop();
				if (const auto topType = GetType(top), underTopType = GetType(underTop); topType != underTopType || topType != Type::Boolean) {
					RaiseException(offset, BinaryTypeException(typeName(underTopType), typeName(topType), u8"그리고"));
					return false;
				}

				frame.Push(BooleanConstant(std::get<BooleanConstant>(underTop).Value && std::get<BooleanConstant>(top).Value));
				break;
			}

			case OpCode::LogicOr: {
				Constant& top = frame.GetTop();
				Constant& underTop = frame.GetUnderTop();
				if (const auto topType = GetType(top), underTopType = GetType(underTop); topType != underTopType || topType != Type::Boolean) {
					RaiseException(offset, BinaryTypeException(typeName(underTopType), typeName(topType), u8"또는"));
					return false;
				}

				frame.Push(BooleanConstant(std::get<BooleanConstant>(underTop).Value || std::get<BooleanConstant>(top).Value));
				break;
			}
			}
		}

		if (m_StackTrace.size() > 1) {
			offset = m_StackTrace[m_StackTrace.size() - 2].GetCurrentOffset() + 1;
			m_StackTrace.erase(m_StackTrace.end() - 1);
			goto start;
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
	std::string Interpreter::UndefinedException(const std::string_view& type, const std::string_view& name) {
		std::string result(type);
		result += u8" '";
		result += name;
		result += u8"'을/를 찾을 수 없습니다.";
		return result;
	}
	std::string Interpreter::NoJosaException(const std::string_view& name) {
		std::string result(u8"함수 '");
		result += name;
		result += u8"'에는 값을 적용할 수 없습니다.";
		return result;
	}
	std::string Interpreter::AlreadyBoundException(const std::string_view& type, const std::string_view& name) {
		std::string result(type);
		result += u8" '";
		result += name;
		result += u8"'에는 이미 값이 적용되어 있습니다.";
		return result;
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