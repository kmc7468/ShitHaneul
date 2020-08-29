#include <ShitHaneul/Function.hpp>

#include <utility>

namespace ShitHaneul {
	FunctionInfo::FunctionInfo(FunctionInfo&& functionInfo) noexcept
		: Name(std::move(functionInfo.Name)), Path(std::move(functionInfo.Path)), Line(functionInfo.Line),
		StackOperandCount(functionInfo.StackOperandCount), LocalVariableCount(functionInfo.LocalVariableCount),
		ConstantList(std::move(functionInfo.ConstantList)), GlobalList(std::move(functionInfo.GlobalList)),
		JosaMap(std::move(functionInfo.JosaMap)) {}

	FunctionInfo& FunctionInfo::operator=(FunctionInfo&& functionInfo) noexcept {
		Name = std::move(functionInfo.Name);
		Path = std::move(functionInfo.Path);
		Line = functionInfo.Line;

		StackOperandCount = functionInfo.StackOperandCount;
		LocalVariableCount = functionInfo.LocalVariableCount;
		ConstantList = std::move(functionInfo.ConstantList);
		GlobalList = std::move(functionInfo.GlobalList);

		JosaMap = std::move(functionInfo.JosaMap);
		return *this;
	}
}

namespace ShitHaneul {
	Function::Function(const FunctionInfo* info)
		: Info(info), JosaMap(info->JosaMap) {}
	Function::Function(Function&& function) noexcept
		: Info(function.Info), JosaMap(std::move(function.JosaMap)), FreeVariableList(std::move(function.FreeVariableList)) {}

	Function& Function::operator=(Function&& function) noexcept {
		Info = function.Info;
		JosaMap = std::move(function.JosaMap);
		FreeVariableList = std::move(function.FreeVariableList);
		return *this;
	}
}