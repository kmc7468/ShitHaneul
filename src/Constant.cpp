#include <ShitHaneul/Constant.hpp>

namespace ShitHaneul {
	IntegerConstant::IntegerConstant(std::int_fast64_t value) noexcept
		: Value(value) {}
	IntegerConstant::IntegerConstant(const IntegerConstant& constant) noexcept
		: Value(constant.Value) {}

	IntegerConstant& IntegerConstant::operator=(const IntegerConstant& constant) noexcept {
		Value = constant.Value;
		return *this;
	}
}

namespace ShitHaneul {
	RealConstant::RealConstant(double value) noexcept
		: Value(value) {}
	RealConstant::RealConstant(const RealConstant& constant) noexcept
		: Value(constant.Value) {}

	RealConstant& RealConstant::operator=(const RealConstant& constant) noexcept {
		Value = constant.Value;
		return *this;
	}
}

namespace ShitHaneul {
	BooleanConstant::BooleanConstant(bool value) noexcept
		: Value(value) {}
	BooleanConstant::BooleanConstant(const BooleanConstant& constant) noexcept
		: Value(constant.Value) {}

	BooleanConstant& BooleanConstant::operator=(const BooleanConstant& constant) noexcept {
		Value = constant.Value;
		return *this;
	}
}

namespace ShitHaneul {
	CharacterConstant::CharacterConstant(char32_t value) noexcept
		: Value(value) {}
	CharacterConstant::CharacterConstant(const CharacterConstant& constant) noexcept
		: Value(constant.Value) {}

	CharacterConstant& CharacterConstant::operator=(const CharacterConstant& constant) noexcept {
		Value = constant.Value;
		return *this;
	}
}