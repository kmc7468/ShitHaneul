#include <ShitHaneul/Function.hpp>

#include <utility>

namespace ShitHaneul {
	FunctionInfo::FunctionInfo(FunctionInfo&& functionInfo) noexcept
		: Name(std::move(functionInfo.Name)), Path(std::move(functionInfo.Path)), Line(functionInfo.Line),
		StackOperandCount(functionInfo.StackOperandCount), LocalVariableCount(functionInfo.LocalVariableCount), ConstantList(std::move(functionInfo.ConstantList)) {}

	FunctionInfo& FunctionInfo::operator=(FunctionInfo&& functionInfo) noexcept {
		Name = std::move(functionInfo.Name);
		Path = std::move(functionInfo.Path);
		Line = functionInfo.Line;

		StackOperandCount = functionInfo.StackOperandCount;
		LocalVariableCount = functionInfo.LocalVariableCount;
		ConstantList = std::move(functionInfo.ConstantList);
		return *this;
	}
}