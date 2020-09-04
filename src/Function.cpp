#include <ShitHaneul/Function.hpp>

#include <cstddef>
#include <utility>

namespace ShitHaneul {
	LineMap::LineMap(LineMap&& lineMap) noexcept
		: m_Map(std::move(lineMap.m_Map)) {}

	LineMap& LineMap::operator=(LineMap&& lineMap) noexcept {
		m_Map = std::move(lineMap.m_Map);
		return *this;
	}
	LineInfo LineMap::operator[](std::uint64_t offset) const {
		auto line = m_Map.begin();
		auto path = m_Map.begin();

		for (auto iter = m_Map.begin(); iter < m_Map.end(); ++iter) {
			if (offset < iter->Offset) break;

			if (iter->Path.empty()) {
				line = iter;
			} else {
				path = iter;
			}
		}
		return { UINT64_MAX, line->Line, path->Path };
	}

	void LineMap::Add(std::uint64_t offset, std::uint16_t line) {
		m_Map.push_back({ offset, line });
	}
	void LineMap::Add(std::uint64_t offset, std::u32string&& path) {
		m_Map.push_back({ offset, UINT16_MAX, std::move(path) });
	}
	std::uint64_t LineMap::GetCount() const noexcept {
		return static_cast<std::uint64_t>(m_Map.size());
	}
	void LineMap::Reserve(std::uint64_t count) {
		m_Map.reserve(static_cast<std::size_t>(count));
	}
}

namespace ShitHaneul {
	FunctionInfo::FunctionInfo(StringList&& josaList, std::function<Constant(std::uint64_t, const StringMap&)>&& builtinFunction)
		: JosaList(std::move(josaList)), BuiltinFunction(std::move(builtinFunction)) {}
	FunctionInfo::FunctionInfo(FunctionInfo&& functionInfo) noexcept
		: Name(std::move(functionInfo.Name)), Line(std::move(functionInfo.Line)),
		StackOperandCount(functionInfo.StackOperandCount), LocalVariableCount(functionInfo.LocalVariableCount),
		ConstantList(std::move(functionInfo.ConstantList)), GlobalList(std::move(functionInfo.GlobalList)),
		JosaList(std::move(functionInfo.JosaList)), LineMap(std::move(functionInfo.LineMap)),
		BuiltinFunction(std::move(functionInfo.BuiltinFunction)), InstructionList(std::move(functionInfo.InstructionList)) {}

	FunctionInfo& FunctionInfo::operator=(FunctionInfo&& functionInfo) noexcept {
		Name = std::move(functionInfo.Name);
		Line = std::move(functionInfo.Line);

		StackOperandCount = functionInfo.StackOperandCount;
		LocalVariableCount = functionInfo.LocalVariableCount;
		ConstantList = std::move(functionInfo.ConstantList);
		GlobalList = std::move(functionInfo.GlobalList);

		JosaList = std::move(functionInfo.JosaList);
		LineMap = std::move(functionInfo.LineMap);
		BuiltinFunction = std::move(functionInfo.BuiltinFunction);
		InstructionList = std::move(functionInfo.InstructionList);
		return *this;
	}
}

namespace ShitHaneul {
	Function::Function(const FunctionInfo* info)
		: Info(info), JosaMap(info->JosaList) {}
	Function::Function(const Function& function)
		: Info(function.Info), JosaMap(function.JosaMap), FreeVariableList(function.FreeVariableList) {}
	Function::Function(Function&& function) noexcept
		: Info(function.Info), JosaMap(std::move(function.JosaMap)), FreeVariableList(std::move(function.FreeVariableList)) {}

	Function& Function::operator=(const Function& function) noexcept {
		Info = function.Info;
		JosaMap = function.JosaMap;
		FreeVariableList = function.FreeVariableList;
		return *this;
	}
	Function& Function::operator=(Function&& function) noexcept {
		Info = function.Info;
		JosaMap = std::move(function.JosaMap);
		FreeVariableList = std::move(function.FreeVariableList);
		return *this;
	}
}