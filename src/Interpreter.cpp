#include <ShitHaneul/Interpreter.hpp>

#include <ShitHaneul/Memory.hpp>

#include <iostream>
#include <memory>
#include <random>
#include <utility>
#include <variant>

namespace ShitHaneul {
	StackFrame::StackFrame(Function* currentFunction)
		: m_Stack(static_cast<std::size_t>(currentFunction->Info->LocalVariableCount + currentFunction->Info->StackOperandCount + currentFunction->JosaMap.GetCount())),
		m_Top(static_cast<std::size_t>(currentFunction->Info->LocalVariableCount + currentFunction->JosaMap.GetCount())), m_CurrentFunction(currentFunction) {
		const std::uint8_t argCount = currentFunction->JosaMap.GetCount();
		for (std::uint8_t i = 0; i < argCount; ++i) {
			m_Stack[static_cast<std::size_t>(i)] = currentFunction->JosaMap[i].second;
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

		RegisterBuiltinFunctions();
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
				if (frame.GetTop().index() == 0) {
					RaiseException(offset, UndefinedFunctionException());
					return false;
				} else if (const auto type = GetType(frame.GetTop()); type != Type::Function) {
					RaiseException(offset, InvalidTypeException(u8"함수", typeName(type)));
					return false;
				}

				const auto target = std::get<FunctionConstant>(frame.GetTop());
				Function* const newFunc = m_ByteFile.CopyFunction(target.Value);
				if (!newFunc->Info) {
					RaiseException(offset, UndefinedFunctionException());
					return false;
				}
				frame.Pop();
				
				for (std::uint8_t i = 0; i < strListOperand.GetCount(); ++i) {
					if (strListOperand[i].second == U"_") {
						const BoundResult result = newFunc->JosaMap.BindConstant(frame.GetTop());
						if (result != BoundResult::Success) {
							RaiseException(offset,
								result == BoundResult::Undefiend ? NoJosaException(EncodeUTF32ToUTF8(newFunc->Info->Name)) :
								AlreadyBoundException(u8"함수", EncodeUTF32ToUTF8(newFunc->Info->Name)));
							return false;
						}
					} else {
						const BoundResult result = newFunc->JosaMap.BindConstant(strListOperand[i].second, frame.GetTop());
						if (result != BoundResult::Success) {
							RaiseException(offset,
								result == BoundResult::Undefiend ? UndefinedException(u8"조사", EncodeUTF32ToUTF8(strListOperand[i].second)) :
								AlreadyBoundException(u8"조사", EncodeUTF32ToUTF8(strListOperand[i].second)));
							return false;
						}
					}
					frame.Pop();
				}

				if (newFunc->JosaMap.GetUnboundCount()) {
					frame.Push(FunctionConstant(newFunc));
				} else {
					if (newFunc->Info->BuiltinFunction) {
						const Constant result = newFunc->Info->BuiltinFunction(offset, newFunc->JosaMap);
						if (result.index()) {
							frame.Push(result);
						} else return false;
					} else {
						frame.SetCurrentOffset(offset);
						offset = static_cast<std::uint64_t>(-1);
						m_StackTrace.emplace_back(newFunc);
					}
				}
				break;
			}

			case OpCode::AddStruct: {
				const auto& [name, fields] = std::get<std::pair<std::u32string, StringList>>(instruction.Operand);
				m_Structures[name] = fields;
				break;
			}

			case OpCode::MakeStruct: {
				const auto& [name, fields] = std::get<std::pair<std::u32string, StringList>>(instruction.Operand);
				const std::uint8_t expectedFieldCount = fields.GetCount();
				const std::uint8_t givenFieldCount = fields.GetCount();
				if (expectedFieldCount != givenFieldCount) {
					RaiseException(offset, FieldMismatchException(expectedFieldCount, givenFieldCount));
					return false;
				}

				std::unique_ptr<StringMap> structure(new StringMap(fields));
				for (std::uint8_t i = 0; i < givenFieldCount; ++i) {
					if (!fields.Contains(fields[i].second)) {
						RaiseException(offset, UndefinedException(u8"필드", EncodeUTF32ToUTF8(fields[i].second)));
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

				const std::optional<Constant> field = (*target.Value)[strOperand];
				if (field) {
					frame.Push(*field);
					break;
				} else {
					RaiseException(offset, UndefinedException(u8"필드", EncodeUTF32ToUTF8(strOperand)));
					return false;
				}
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
						Constant& variable = frame.LoadDirect(index);
						if (!variable.index()) {
							std::unique_ptr<Function> dummyFunc(new Function);
							m_ByteFile.AddFunction(dummyFunc.get());
							variable = FunctionConstant(dummyFunc.release(), true);
						}
						target.Value->FreeVariableList.push_back(variable);
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
						frame.Pop(); frame.Pop();
						frame.Push(IntegerConstant(std::get<IntegerConstant>(underTop).Value + std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Pop(); frame.Pop();
						frame.Push(RealConstant(std::get<IntegerConstant>(underTop).Value + std::get<RealConstant>(top).Value));
					} else goto addError;
				} else if (underTopType == Type::Real) {
					if (topType == Type::Integer) {
						frame.Pop(); frame.Pop();
						frame.Push(RealConstant(std::get<RealConstant>(underTop).Value + std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Pop(); frame.Pop();
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
						frame.Pop(); frame.Pop();
						frame.Push(IntegerConstant(std::get<IntegerConstant>(underTop).Value - std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Pop(); frame.Pop();
						frame.Push(RealConstant(std::get<IntegerConstant>(underTop).Value - std::get<RealConstant>(top).Value));
					} else goto subError;
				} else if (underTopType == Type::Real) {
					if (topType == Type::Integer) {
						frame.Pop(); frame.Pop();
						frame.Push(RealConstant(std::get<RealConstant>(underTop).Value - std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Pop(); frame.Pop();
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
						frame.Pop(); frame.Pop();
						frame.Push(IntegerConstant(std::get<IntegerConstant>(underTop).Value * std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Pop(); frame.Pop();
						frame.Push(RealConstant(std::get<IntegerConstant>(underTop).Value * std::get<RealConstant>(top).Value));
					} else goto mulError;
				} else if (underTopType == Type::Real) {
					frame.Pop(); frame.Pop();
					if (topType == Type::Integer) {
						frame.Push(RealConstant(std::get<RealConstant>(underTop).Value * std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Pop(); frame.Pop();
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
						frame.Pop(); frame.Pop();
						frame.Push(IntegerConstant(std::get<IntegerConstant>(underTop).Value / std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						if (std::get<RealConstant>(top).Value == 0) goto divZeroError;
						frame.Pop(); frame.Pop();
						frame.Push(RealConstant(std::get<IntegerConstant>(underTop).Value / std::get<RealConstant>(top).Value));
					} else goto divError;
				} else if (underTopType == Type::Real) {
					if (topType == Type::Integer) {
						if (std::get<IntegerConstant>(top).Value == 0) goto divZeroError;
						frame.Pop(); frame.Pop();
						frame.Push(RealConstant(std::get<RealConstant>(underTop).Value / std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						if (std::get<RealConstant>(top).Value == 0) {
						divZeroError:
							RaiseException(offset, DivideByZeroException());
							return false;
						}
						frame.Pop(); frame.Pop();
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
				frame.Pop(); frame.Pop();
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
					frame.Pop(); frame.Pop();
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
						frame.Pop(); frame.Pop();
						frame.Push(BooleanConstant(std::get<IntegerConstant>(underTop).Value < std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Pop(); frame.Pop();
						frame.Push(BooleanConstant(std::get<IntegerConstant>(underTop).Value < std::get<RealConstant>(top).Value));
					} else goto lessThanError;
				} else if (underTopType == Type::Real) {
					if (topType == Type::Integer) {
						frame.Pop(); frame.Pop();
						frame.Push(BooleanConstant(std::get<RealConstant>(underTop).Value < std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Pop(); frame.Pop();
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
						frame.Pop(); frame.Pop();
						frame.Push(BooleanConstant(std::get<IntegerConstant>(underTop).Value > std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Pop(); frame.Pop();
						frame.Push(BooleanConstant(std::get<IntegerConstant>(underTop).Value > std::get<RealConstant>(top).Value));
					} else goto greaterThanError;
				} else if (underTopType == Type::Real) {
					if (topType == Type::Integer) {
						frame.Pop(); frame.Pop();
						frame.Push(BooleanConstant(std::get<RealConstant>(underTop).Value > std::get<IntegerConstant>(top).Value));
					} else if (topType == Type::Real) {
						frame.Pop(); frame.Pop();
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

				frame.Pop(); frame.Pop();
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

				frame.Pop(); frame.Pop();
				frame.Push(BooleanConstant(std::get<BooleanConstant>(underTop).Value || std::get<BooleanConstant>(top).Value));
				break;
			}
			}
		}

		if (m_StackTrace.size() > 1) {
			offset = m_StackTrace[m_StackTrace.size() - 2].GetCurrentOffset() + 1;
			m_StackTrace[m_StackTrace.size() - 2].Push(frame.GetTop());
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

	void Interpreter::RegisterBuiltinFunction(const std::u32string& name, StringList&& josaList,
		std::function<Constant(std::uint64_t, const StringMap&)>&& builtinFunction) {
		std::unique_ptr<FunctionInfo> function(new FunctionInfo(std::move(josaList), std::move(builtinFunction)));
		m_GlobalVariables[name] = FunctionConstant(m_ByteFile.RegisterFunction(function.get()));
		function.release();
	}
	void Interpreter::RegisterBuiltinFunctions() {
		using namespace std::string_literals;
		RegisterBuiltinFunction(U"문자_출력하다", { { U"을"s } }, [&](std::uint64_t offset, const StringMap& arguments) -> Constant {
			if (const auto type = GetType(arguments[0].second); type != Type::Character) {
				RaiseException(offset, InvalidTypeException(u8"문자", typeName(type)));
				return std::monostate{};
			} else {
				WriteCharacterToStdout(std::get<CharacterConstant>(arguments[0].second).Value);
				return NoneConstant{};
			}
		});
		RegisterBuiltinFunction(U"문자열화하다", { { U"을"s } }, [&](std::uint64_t, const StringMap& arguments) -> Constant {
			return ConvertStringToList(ToString(arguments[0].second));
		});
		RegisterBuiltinFunction(U"입력받다", {}, [&](std::uint64_t, const StringMap&) -> Constant {
			std::u32string result;
			do {
				result.push_back(ReadCharacterFromStdin());
			} while (result.back() != '\n');
			result.erase(result.end() - 1);
			return ConvertStringToList(result);
		});
		RegisterBuiltinFunction(U"정수화하다", { { U"을"s } }, [&](std::uint64_t offset, const StringMap& arguments) -> Constant {
			const auto type = GetType(arguments[0].second);
			switch (type) {
			case Type::Integer: return arguments[0].second;
			case Type::Real: return IntegerConstant(static_cast<std::int64_t>(std::get<RealConstant>(arguments[0].second).Value));
			case Type::Character: return IntegerConstant(static_cast<std::int64_t>(std::get<CharacterConstant>(arguments[0].second).Value));
			case Type::Structure: {
				const std::optional<std::u32string> str = ConvertListToString(offset, arguments[0].second);
				if (!str) return std::monostate();
				const std::string strUTF8 = EncodeUTF32ToUTF8(*str);
				return IntegerConstant(static_cast<std::int64_t>(std::stoll(strUTF8)));
			}
			default:
				RaiseException(offset, InvalidTypeException(u8"정수화할 수 있는", typeName(type)));
				return std::monostate{};
			}
		});
		RegisterBuiltinFunction(U"실수화하다", { { U"을"s } }, [&](std::uint64_t offset, const StringMap& arguments) -> Constant {
			const auto type = GetType(arguments[0].second);
			switch (type) {
			case Type::Integer: return RealConstant(static_cast<double>(std::get<IntegerConstant>(arguments[0].second).Value));
			case Type::Real: return arguments[0].second;
			case Type::Character: return RealConstant(static_cast<double>(std::get<CharacterConstant>(arguments[0].second).Value));
			case Type::Structure: {
				const std::optional<std::u32string> str = ConvertListToString(offset, arguments[0].second);
				if (!str) return std::monostate();
				const std::string strUTF8 = EncodeUTF32ToUTF8(*str);
				return RealConstant(std::stod(strUTF8));
			}
			default:
				RaiseException(offset, InvalidTypeException(u8"실수화할 수 있는", typeName(type)));
				return std::monostate{};
			}
		});
		RegisterBuiltinFunction(U"난수_가져오다", {}, [&](std::uint64_t, const StringMap&) -> Constant {
			thread_local std::mt19937_64 mt(std::random_device{}());
			return IntegerConstant(static_cast<std::int64_t>(mt()));
		});
	}
	Constant Interpreter::ConvertStringToList(const std::u32string& string) {
		if (string.empty()) return NoneConstant();

		thread_local const StringList nodeFields = []() {
			StringList nodeFields;
			nodeFields.Add(U"첫번째");
			nodeFields.Add(U"나머지");
			return nodeFields;
		}();
		const StringMap nodeBase(nodeFields);

		std::vector<std::unique_ptr<StringMap>> nodes;
		StringMap* first = nullptr;
		for (std::size_t i = 0; i < string.size(); ++i) {
			std::unique_ptr<StringMap>& node = nodes.emplace_back(new StringMap(nodeBase));
			node->BindConstant(CharacterConstant(string[i]));
			if (i) {
				nodes[i - 1]->BindConstant(StructureConstant(node.get()));
			} else {
				first = node.get();
			}
		}
		nodes.back()->BindConstant(NoneConstant{});

		m_ByteFile.AllocateStructures(nodes.size());
		for (std::unique_ptr<StringMap>& node : nodes) {
			m_ByteFile.AddStructure(node.release());
		}
		return StructureConstant(first);
	}
	std::optional<std::u32string> Interpreter::ConvertListToString(std::uint64_t offset, const Constant& list) {
		std::u32string result;

		const Constant* current = &list;
		Type currentType;
		while ((currentType = GetType(*current)) != Type::None) {
			if (currentType != Type::Structure) {
				RaiseException(offset, InvalidTypeException(u8"구조체", typeName(currentType)));
				return std::nullopt;
			}

			const auto node = std::get<StructureConstant>(*current);
			const std::optional<Constant> item = (*node.Value)[U"첫번째"];
			if (item) {
				if (const auto itemType = GetType(*item); itemType != Type::Character) {
					RaiseException(offset, InvalidTypeException(u8"문자", typeName(currentType)));
					return std::nullopt;
				}
				result.push_back(std::get<CharacterConstant>(*item).Value);
			} else {
				RaiseException(offset, UndefinedException(u8"필드", u8"첫번째"));
				return std::nullopt;
			}
		}
		return result;
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
	std::string Interpreter::UndefinedFunctionException() {
		return u8"선언은 되었으나 정의되지 않은 함수를 호출할 수 없습니다.";
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

namespace ShitHaneul {
	char32_t EncodeUTF8ToUTF32(char first, char second, char third, char fourth) noexcept {
		if (second == 0) return static_cast<char32_t>(first);
		else if (third == 0) return ((static_cast<char32_t>(first) & 0x1F) << 6) + (static_cast<char32_t>(second) & 0x3F);
		else if (fourth == 0) return ((static_cast<char32_t>(first) & 0x0F) << 12) + ((static_cast<char32_t>(second) & 0x3F) << 6) +
			(static_cast<char32_t>(third) & 0x3F);
		else return ((static_cast<char32_t>(first) & 0x07) << 18) + ((static_cast<char32_t>(second) & 0x3F) << 12) +
			((static_cast<char32_t>(second) & 0x3F) << 6) + (static_cast<char32_t>(fourth) & 0x3F);
	}
	std::u32string EncodeUTF8ToUTF32(const std::string_view& utf8) {
		std::u32string result;

		for (std::size_t i = 0; i < utf8.size();) {
			const auto first = static_cast<unsigned char>(utf8[i]);
			if (first < 0x80) {
				result.push_back(EncodeUTF8ToUTF32(utf8[i], 0, 0, 0));
				i += 1;
			} else if (first < 0xE0) {
				result.push_back(EncodeUTF8ToUTF32(utf8[i], utf8[i + 1], 0, 0));
				i += 2;
			} else if (first < 0xF0) {
				result.push_back(EncodeUTF8ToUTF32(utf8[i], utf8[i + 1], utf8[i + 2], 0));
				i += 3;
			} else {
				result.push_back(EncodeUTF8ToUTF32(utf8[i], utf8[i + 1], utf8[i + 2], utf8[i + 3]));
				i += 4;
			}
		}
		return result;
	}
	std::string EncodeUTF32ToUTF8(char32_t character) {
		std::string result;

		if (character < 0x80) {
			result.push_back(static_cast<char>(character));
		} else if (character < 0x0800) {
			result.push_back(static_cast<char>(0xC0 | (character >> 6)));
			result.push_back(static_cast<char>(0x80 | (character & 0x3F)));
		} else if (character < 0x10000) {
			result.push_back(static_cast<char>(0xE0 | (character >> 12)));
			result.push_back(static_cast<char>(0x80 | ((character >> 6) & 0x3F)));
			result.push_back(static_cast<char>(0x80 | (character & 0x3F)));
		} else {
			result.push_back(static_cast<char>(0xF0 | (character >> 18)));
			result.push_back(static_cast<char>(0x80 | ((character >> 12) & 0x3F)));
			result.push_back(static_cast<char>(0x80 | ((character >> 6) & 0x3F)));
			result.push_back(static_cast<char>(0x80 | (character & 0x3F)));
		}
		return result;
	}
	std::string EncodeUTF32ToUTF8(const std::u32string_view& utf32) {
		std::string result;

		for (char32_t c : utf32) {
			const std::string temp = EncodeUTF32ToUTF8(c);
			result.insert(result.end(), temp.begin(), temp.end());
		}
		return result;
	}

	char32_t ReadCharacterFromStdin() {
#ifdef SHN_WINDOWS
		wchar_t first;
		std::wcin >> first;
		if (0xD800 <= first && first <= 0xDBFF) {
			wchar_t second;
			std::wcin >> second;
			return (static_cast<char32_t>(first) - 0xD800) * 0x400 + (static_cast<char32_t>(second) - 0xDC00);
		} else return static_cast<char32_t>(first);
#else
		char first;
		std::cin >> first;
		const auto first = static_cast<unsigned char>(utf8[i]);
		if (static_cast<unsigned char>(first) < 0x80) return EncodeUTF8ToUTF32(first, 0, 0, 0);
		else if (static_cast<unsigned char>(first) < 0xE0) {
			char second;
			std::cin >> second;
			return EncodeUTF8ToUTF32(first, second, 0, 0);
		} else if (static_cast<unsigned char>(first) < 0xF0) {
			char second, third;
			std::cin >> second >> third;
			return EncodeUTF8ToUTF32(first, second, third, 0);
		} else {
			char second, third, fourth;
			std::cin >> second >> third >> fourth;
			result.push_back(EncodeUTF8ToUTF32(first, second, third, fourth));
		}
#endif
	}
	void WriteCharacterToStdout(char32_t character) {
#ifdef SHN_WINDOWS
		if (character < 0x10000) {
			std::wcout << static_cast<wchar_t>(character);
		} else {
			character -= 0x10000;
			const wchar_t highSurrogate = static_cast<wchar_t>((character / 0x400) + 0xD800);
			const wchar_t lowSurrogate = static_cast<wchar_t>((character % 0x400) + 0xDC00);
			std::wcout << highSurrogate << lowSurrogate;
		}
#else
		std::cout << EncodeUTF32ToUTF8(character);
#endif
	}
	void WriteStringToStdout(const std::string_view& string) {
#ifdef SHN_WINDOWS
		for (std::size_t i = 0; i < string.size();) {
			const auto first = static_cast<unsigned char>(string[i]);
			char32_t c;
			if (first < 0x80) {
				c = EncodeUTF8ToUTF32(string[i], 0, 0, 0);
				i += 1;
			} else if (first < 0xE0) {
				c = EncodeUTF8ToUTF32(string[i], string[i + 1], 0, 0);
				i += 2;
			} else if (first < 0xF0) {
				c = EncodeUTF8ToUTF32(string[i], string[i + 1], string[i + 2], 0);
				i += 3;
			} else {
				c = EncodeUTF8ToUTF32(string[i], string[i + 1], string[i + 2], string[i + 3]);
				i += 4;
			}

			if (c < 0x10000) {
				std::wcout << static_cast<wchar_t>(c);
			} else {
				c -= 0x10000;
				const wchar_t highSurrogate = static_cast<wchar_t>((c / 0x400) + 0xD800);
				const wchar_t lowSurrogate = static_cast<wchar_t>((c % 0x400) + 0xDC00);
				std::wcout << highSurrogate << lowSurrogate;
			}
		}
#else
		std::cout << string;
#endif
	}
}