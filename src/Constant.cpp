#include <ShitHaneul/Constant.hpp>

#include <algorithm>
#include <cassert>
#include <functional>

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
	StructureConstant::StructureConstant(JosaMap* value) noexcept
		: Value(value) {}
	StructureConstant::StructureConstant(const StructureConstant& constant) noexcept
		: Value(constant.Value) {}

	StructureConstant& StructureConstant::operator=(const StructureConstant& constant) noexcept {
		Value = constant.Value;
		return *this;
	}
}

namespace ShitHaneul {
	Type GetType(const Constant& constant) noexcept {
		assert(constant.index() > 0);
		return static_cast<Type>(constant.index() - 1);
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

	void ConstantList::Add() {
		m_List.push_back(NoneConstant{});
	}
	std::uint64_t ConstantList::GetCount() const noexcept {
		return static_cast<std::uint64_t>(m_List.size());
	}
	void ConstantList::Reserve(std::uint64_t count) {
		m_List.reserve(static_cast<std::size_t>(count));
	}
}

namespace ShitHaneul {
	JosaList::JosaList(JosaList&& josaList) noexcept
		: m_List(std::move(josaList.m_List)) {}

	JosaList& JosaList::operator=(JosaList&& josaList) noexcept {
		m_List = std::move(josaList.m_List);
		return *this;
	}
	const std::pair<std::size_t, std::u32string>& JosaList::operator[](std::uint8_t index) const noexcept {
		return m_List[static_cast<std::size_t>(index)];
	}

	void JosaList::Add(std::u32string&& josa) {
		const std::size_t hash = std::hash<std::u32string>{}(josa);
		m_List.push_back({ hash, std::move(josa) });
	}
	std::uint8_t JosaList::GetCount() const noexcept {
		return static_cast<std::uint8_t>(m_List.size());
	}
	void JosaList::Reserve(std::uint8_t count) {
		m_List.reserve(static_cast<std::size_t>(count));
	}
}

namespace ShitHaneul {
	JosaMap::JosaMap(const JosaList& josaList) {
		const std::uint8_t count = josaList.GetCount();
		m_Map.reserve(static_cast<std::size_t>(count));

		for (std::uint8_t i = 0; i < count; ++i) {
			m_Map.push_back({ josaList[i], {} });
		}
	}
	JosaMap::JosaMap(const JosaMap& josaMap) noexcept
		: m_Map(josaMap.m_Map) {}
	JosaMap::JosaMap(JosaMap&& josaMap) noexcept
		: m_Map(std::move(josaMap.m_Map)) {}

	JosaMap& JosaMap::operator=(const JosaMap& josaMap) noexcept {
		m_Map = josaMap.m_Map;
		return *this;
	}
	JosaMap& JosaMap::operator=(JosaMap&& josaMap) noexcept {
		m_Map = std::move(josaMap.m_Map);
		return *this;
	}

	void JosaMap::BindConstant(const std::u32string_view& josa, const Constant& constant) {
		const std::size_t hash = std::hash<std::u32string_view>{}(josa);
		const auto iter = std::find_if(m_Map.begin(), m_Map.end(), [josa, hash](const auto& element) {
			return element.first.first == hash && element.first.second == josa;
		});
		iter->second = constant;
	}
}