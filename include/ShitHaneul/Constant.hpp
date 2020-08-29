#pragma once

#include <cstdint>
#include <type_traits>
#include <variant>

namespace ShitHaneul {
	enum class Type {
		None,
		Integer,
		Real,
		Boolean,
		Character,
		Function,
		Structure,
	};
}

namespace ShitHaneul {
	class IntegerConstant final {
	public:
		const ShitHaneul::Type Type = ShitHaneul::Type::Integer;
		std::int64_t Value = 0;

	public:
		IntegerConstant() noexcept = default;
		IntegerConstant(std::int64_t value) noexcept;
		IntegerConstant(const IntegerConstant& constant) noexcept;
		~IntegerConstant() = default;

	public:
		IntegerConstant& operator=(const IntegerConstant& constant) noexcept;
	};
}

namespace ShitHaneul {
	class RealConstant final {
	public:
		const ShitHaneul::Type Type = ShitHaneul::Type::Real;
		double Value = 0;

	public:
		RealConstant() noexcept = default;
		RealConstant(double value) noexcept;
		RealConstant(const RealConstant& constant) noexcept;
		~RealConstant() = default;

	public:
		RealConstant& operator=(const RealConstant& constant) noexcept;
	};
}

namespace ShitHaneul {
	class BooleanConstant final {
	public:
		const ShitHaneul::Type Type = ShitHaneul::Type::Boolean;
		bool Value = 0;

	public:
		BooleanConstant() noexcept = default;
		BooleanConstant(bool value) noexcept;
		BooleanConstant(const BooleanConstant& constant) noexcept;
		~BooleanConstant() = default;

	public:
		BooleanConstant& operator=(const BooleanConstant& constant) noexcept;
	};
}

namespace ShitHaneul {
	class CharacterConstant final {
	public:
		const ShitHaneul::Type Type = ShitHaneul::Type::Character;
		char32_t Value = 0;

	public:
		CharacterConstant() noexcept = default;
		CharacterConstant(char32_t value) noexcept;
		CharacterConstant(const CharacterConstant& constant) noexcept;
		~CharacterConstant() = default;

	public:
		CharacterConstant& operator=(const CharacterConstant& constant) noexcept;
	};
}

namespace ShitHaneul {
	using Constant = std::variant<std::monostate,
		IntegerConstant,
		RealConstant,
		BooleanConstant,
		CharacterConstant>;

	template<typename T>
	using MakeConstantClass = std::conditional_t<
		std::is_same_v<T, std::int64_t>, IntegerConstant,
		std::conditional_t<
			std::is_same_v<T, double>, RealConstant,
			std::conditional_t<
				std::is_same_v<T, bool>, BooleanConstant,
				std::conditional_t<
					std::is_same_v<T, char32_t>, CharacterConstant,
					void
				>
			>
		>
	>;
}