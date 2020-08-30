#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace ShitHaneul {
	enum class Type : std::uint8_t {
		None,
		Integer,
		Real,
		Boolean,
		Character,
		Function,
		Structure,
	};

	class Function;
	class JosaMap;

	class NoneConstant final {
	public:
		NoneConstant() noexcept = default;
		NoneConstant(const NoneConstant& constant) noexcept;
		~NoneConstant() = default;

	public:
		NoneConstant& operator=(const NoneConstant& constant) noexcept;
	};

	class IntegerConstant final {
		std::int64_t Value = 0;

	public:
		IntegerConstant() noexcept = default;
		IntegerConstant(std::int64_t value) noexcept;
		IntegerConstant(const IntegerConstant& constant) noexcept;
		~IntegerConstant() = default;

	public:
		IntegerConstant& operator=(const IntegerConstant& constant) noexcept;
	};

	class RealConstant final {
		double Value = 0.0;

	public:
		RealConstant() noexcept = default;
		RealConstant(double value) noexcept;
		RealConstant(const RealConstant& constant) noexcept;
		~RealConstant() = default;

	public:
		RealConstant& operator=(const RealConstant& constant) noexcept;
	};

	class BooleanConstant final {
		bool Value = false;

	public:
		BooleanConstant() noexcept = default;
		BooleanConstant(bool value) noexcept;
		BooleanConstant(const BooleanConstant& constant) noexcept;
		~BooleanConstant() = default;

	public:
		BooleanConstant& operator=(const BooleanConstant& constant) noexcept;
	};

	class CharacterConstant final {
		char32_t Value = '\0';

	public:
		CharacterConstant() noexcept = default;
		CharacterConstant(char32_t value) noexcept;
		CharacterConstant(const CharacterConstant& constant) noexcept;
		~CharacterConstant() = default;

	public:
		CharacterConstant& operator=(const CharacterConstant& constant) noexcept;
	};

	class FunctionConstant final {
		Function* Value = nullptr;

	public:
		FunctionConstant() noexcept = default;
		FunctionConstant(Function* value) noexcept;
		FunctionConstant(const FunctionConstant& constant) noexcept;
		~FunctionConstant() = default;

	public:
		FunctionConstant& operator=(const FunctionConstant& constant) noexcept;
	};

	class StructureConstant final {
		JosaMap* Value = nullptr;

	public:
		StructureConstant() noexcept = default;
		StructureConstant(JosaMap* value) noexcept;
		StructureConstant(const StructureConstant& constant) noexcept;
		~StructureConstant() = default;

	public:
		StructureConstant& operator=(const StructureConstant& constant) noexcept;
	};
}

namespace ShitHaneul {
	using Constant = std::variant<std::monostate,
		NoneConstant,
		IntegerConstant,
		RealConstant,
		BooleanConstant,
		CharacterConstant,
		FunctionConstant,
		StructureConstant>;

	Type GetType(const Constant& constant) noexcept;

	template<typename T>
	using MakeConstantClass = std::conditional_t<
		std::is_same_v<T, std::int64_t>, IntegerConstant, std::conditional_t<
		std::is_same_v<T, double>, RealConstant, std::conditional_t<
		std::is_same_v<T, bool>, BooleanConstant, std::conditional_t<
		std::is_same_v<T, char32_t>, CharacterConstant, std::conditional_t<
		std::is_same_v<T, Function*>, FunctionConstant, std::conditional_t<
		std::is_same_v<T, JosaMap*>, StructureConstant,
		void
		>>>>>>;

	class ConstantList final {
	private:
		std::vector<Constant> m_List;

	public:
		ConstantList() noexcept = default;
		ConstantList(ConstantList&& constantList) noexcept;
		~ConstantList() = default;

	public:
		ConstantList& operator=(ConstantList&& constantList) noexcept;
		Constant operator[](std::uint64_t index) const noexcept;

	public:
		void Add();
		template<typename T>
		void Add(T rawConstant);
		std::uint64_t GetCount() const noexcept;
		void Reserve(std::uint64_t count);
	};
}

namespace ShitHaneul {
	class JosaList final {
	private:
		std::vector<std::pair<std::size_t, std::u32string>> m_List;

	public:
		JosaList() noexcept = default;
		JosaList(JosaList&& josaList) noexcept;
		~JosaList() = default;

	public:
		JosaList& operator=(JosaList&& josaList) noexcept;
		const std::pair<std::size_t, std::u32string>& operator[](std::uint8_t index) const noexcept;

	public:
		void Add(std::u32string&& josa);
		std::uint8_t GetCount() const noexcept;
		void Reserve(std::uint8_t count);
	};
}

namespace ShitHaneul {
	class JosaMap final {
	private:
		std::vector<std::pair<std::pair<std::size_t, std::u32string_view>, Constant>> m_Map;

	public:
		explicit JosaMap(const JosaList& josaList);
		JosaMap(const JosaMap& josaMap) noexcept;
		JosaMap(JosaMap&& josaMap) noexcept;
		~JosaMap() = default;

	public:
		JosaMap& operator=(const JosaMap& josaMap) noexcept;
		JosaMap& operator=(JosaMap&& josaMap) noexcept;

	public:
		void BindConstant(const std::u32string_view& josa, const Constant& constant);
	};
}

#include "impl/Constant.hpp"