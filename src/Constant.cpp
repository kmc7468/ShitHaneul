#include <ShitHaneul/Constant.hpp>

#include <cstddef>
#include <utility>

namespace ShitHaneul {
	NoneConstant::NoneConstant(const NoneConstant&) noexcept {}

	NoneConstant& NoneConstant::operator=(const NoneConstant&) noexcept {
		return *this;
	}
}

namespace ShitHaneul {
	IntegerConstant::IntegerConstant(std::int64_t value) noexcept
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

namespace ShitHaneul {
	FunctionConstant::FunctionConstant(Function* value) noexcept
		: Value(value) {}
	FunctionConstant::FunctionConstant(const FunctionConstant& constant) noexcept
		: Value(constant.Value) {}

	FunctionConstant& FunctionConstant::operator=(const FunctionConstant& constant) noexcept {
		Value = constant.Value;
		return *this;
	}
}

namespace ShitHaneul {
	ConstantList::ConstantList(ConstantList&& constantList) noexcept
		: m_List(std::move(constantList.m_List)) {}

	ConstantList& ConstantList::operator=(ConstantList&& constantList) noexcept {
		m_List = std::move(constantList.m_List);
		return *this;
	}
	Constant ConstantList::operator[](std::uint64_t index) const noexcept {
		return m_List[static_cast<std::size_t>(index)];
	}

	std::uint64_t ConstantList::GetCount() const noexcept {
		return static_cast<std::uint64_t>(m_List.size());
	}
	void ConstantList::Reserve(std::uint64_t count) {
		m_List.reserve(static_cast<std::size_t>(count));
	}
}