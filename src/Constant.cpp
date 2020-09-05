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
	FunctionConstant::FunctionConstant(Function* value) noexcept
		: Value(value) {}
	FunctionConstant::FunctionConstant(Function* value, bool isForwardDeclared) noexcept
		: Value(value), IsForwardDeclared(isForwardDeclared) {}
	FunctionConstant::FunctionConstant(const FunctionConstant& constant) noexcept
		: Value(constant.Value), IsForwardDeclared(constant.IsForwardDeclared) {}

	FunctionConstant& FunctionConstant::operator=(const FunctionConstant& constant) noexcept {
		Value = constant.Value;
		IsForwardDeclared = constant.IsForwardDeclared;
		return *this;
	}
}

namespace ShitHaneul {
	StructureConstant::StructureConstant(StringMap* value) noexcept
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
	bool Equal(const Constant& lhs, const Constant& rhs) noexcept {
		const auto lhsType = GetType(lhs), rhsType = GetType(rhs);
		switch (lhsType) {
		case Type::None: return lhsType == rhsType;
		case Type::Integer:
			if (rhsType == Type::Integer) return std::get<IntegerConstant>(lhs).Value == std::get<IntegerConstant>(rhs).Value;
			else if (rhsType == Type::Real) return std::get<IntegerConstant>(lhs).Value == std::get<RealConstant>(rhs).Value;
			else return false;
		case Type::Real:
			if (rhsType == Type::Integer) return std::get<RealConstant>(lhs).Value == std::get<IntegerConstant>(rhs).Value;
			else if (rhsType == Type::Real) return std::get<RealConstant>(lhs).Value == std::get<RealConstant>(rhs).Value;
			else return false;
		case Type::Boolean:
			if (rhsType == Type::Boolean) return std::get<BooleanConstant>(lhs).Value == std::get<BooleanConstant>(rhs).Value;
			else return false;
		case Type::Character:
			if (rhsType == Type::Character) return std::get<CharacterConstant>(lhs).Value == std::get<CharacterConstant>(rhs).Value;
			else return false;
		case Type::Function: return false;
		case Type::Structure: return lhsType == rhsType && *std::get<StructureConstant>(lhs).Value == *std::get<StructureConstant>(rhs).Value;
		}
	}
	std::u32string ToString(const Constant& constant) {
		const auto type = GetType(constant);
		switch (type) {
		case Type::None: return U"(없음)";
		case Type::Integer: {
			const std::string temp = std::to_string(std::get<IntegerConstant>(constant).Value);
			return { temp.begin(), temp.end() };
		}
		case Type::Real: {
			const std::string temp = std::to_string(std::get<RealConstant>(constant).Value);
			return { temp.begin(), temp.end() };
		}
		case Type::Boolean: return std::get<BooleanConstant>(constant).Value ? U"참" : U"거짓";
		case Type::Character: {
			std::u32string result(3, U'\'');
			result[1] = std::get<CharacterConstant>(constant).Value;
			return result;
		}
		case Type::Function: return U"(함수)";
		case Type::Structure: {
			std::u32string result(1, U'{');

			const auto structure = std::get<StructureConstant>(constant);
			const std::uint8_t count = structure.Value->GetCount();
			for (std::uint8_t i = 0; i < count; ++i) {
				if (i) {
					result += U", ";
				}

				const auto [name, value] = (*structure.Value)[i];
				result += name;
				result += U": ";
				result += ToString(value);
			}
			return result += U'}';
		}
		}
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
	StringList::StringList(std::vector<std::u32string>&& list) {
		for (std::u32string& string : list) {
			Add(std::move(string));
		}
	}
	StringList::StringList(const StringList& stringList)
		: m_List(stringList.m_List) {}
	StringList::StringList(StringList&& stringList) noexcept
		: m_List(std::move(stringList.m_List)) {}

	StringList& StringList::operator=(const StringList& stringList) {
		m_List = stringList.m_List;
		return *this;
	}
	StringList& StringList::operator=(StringList&& stringList) noexcept {
		m_List = std::move(stringList.m_List);
		return *this;
	}
	const std::pair<std::size_t, std::u32string>& StringList::operator[](std::uint8_t index) const noexcept {
		return m_List[static_cast<std::size_t>(index)];
	}

	void StringList::Add(std::u32string&& string) {
		const std::size_t hash = std::hash<std::u32string>{}(string);
		m_List.push_back({ hash, std::move(string) });
	}
	std::uint8_t StringList::GetCount() const noexcept {
		return static_cast<std::uint8_t>(m_List.size());
	}
	void StringList::Reserve(std::uint8_t count) {
		m_List.reserve(static_cast<std::size_t>(count));
	}
	bool StringList::Contains(const std::u32string_view& string) const noexcept {
		const std::size_t hash = std::hash<std::u32string_view>{}(string);
		return std::find_if(m_List.begin(), m_List.end(), [string, hash](const auto& element) {
			return element.first == hash && element.second == string;
		}) != m_List.end();
	}
}

namespace ShitHaneul {
	StringMap::StringMap(const StringList& stringList) {
		const std::uint8_t count = stringList.GetCount();
		m_Map.reserve(static_cast<std::size_t>(count));

		for (std::uint8_t i = 0; i < count; ++i) {
			m_Map.push_back({ stringList[i], {} });
		}
	}
	StringMap::StringMap(const StringMap& stringMap) noexcept
		: m_Map(stringMap.m_Map), m_BoundCount(stringMap.m_BoundCount) {}
	StringMap::StringMap(StringMap&& stringMap) noexcept
		: m_Map(std::move(stringMap.m_Map)), m_BoundCount(stringMap.m_BoundCount) {}

	StringMap& StringMap::operator=(const StringMap& stringMap) noexcept {
		m_Map = stringMap.m_Map;
		m_BoundCount = stringMap.m_BoundCount;
		return *this;
	}
	StringMap& StringMap::operator=(StringMap&& stringMap) noexcept {
		m_Map = std::move(stringMap.m_Map);
		m_BoundCount = stringMap.m_BoundCount;
		return *this;
	}
	bool StringMap::operator==(const StringMap& other) const noexcept {
		if (m_Map.size() != other.m_Map.size() || m_BoundCount != other.m_BoundCount) return false;
		for (std::size_t i = 0; i < m_Map.size(); ++i) {
			if (m_Map[i].first.first != other.m_Map[i].first.first ||
				m_Map[i].first.second != other.m_Map[i].first.second) return false;
			else if (!Equal(m_Map[i].second, other.m_Map[i].second)) return false;
		}
		return true;
	}
	bool StringMap::operator!=(const StringMap& other) const noexcept {
		return !(*this == other);
	}
	std::pair<std::u32string_view, Constant> StringMap::operator[](std::uint8_t index) const noexcept {
		const auto& item = m_Map[static_cast<std::size_t>(index)];
		return { item.first.second, item.second };
	}
	std::optional<Constant> StringMap::operator[](const std::u32string_view& string) const noexcept {
		const std::size_t hash = std::hash<std::u32string_view>{}(string);
		const auto iter = std::find_if(m_Map.begin(), m_Map.end(), [string, hash](const auto& element) {
			return element.first.first == hash && element.first.second == string;
		});
		if (iter != m_Map.end()) return iter->second;
		else return std::nullopt;
	}

	bool StringMap::IsEmpty() const noexcept {
		return m_Map.empty();
	}
	std::uint8_t StringMap::GetCount() const noexcept {
		return static_cast<std::uint8_t>(m_Map.size());
	}
	std::uint8_t StringMap::GetBoundCount() const noexcept {
		return m_BoundCount;
	}
	std::uint8_t StringMap::GetUnboundCount() const noexcept {
		return GetCount() - GetBoundCount();
	}
	BoundResult StringMap::BindConstant(const Constant& constant) {
		const auto iter = std::find_if(m_Map.begin(), m_Map.end(), [](const auto& element) {
			return !element.second.index();
		});

		if (iter == m_Map.end()) {
			if (m_Map.size() == 0) return BoundResult::Undefiend;
			else return BoundResult::AlreadyBound;
		}

		iter->second = constant;
		++m_BoundCount;
		return BoundResult::Success;
	}
	BoundResult StringMap::BindConstant(const std::u32string_view& string, const Constant& constant) {
		const std::size_t hash = std::hash<std::u32string_view>{}(string);
		const auto iter = std::find_if(m_Map.begin(), m_Map.end(), [string, hash](const auto& element) {
			return element.first.first == hash && element.first.second == string;
		});

		if (iter == m_Map.end()) return BoundResult::Undefiend;
		else if (iter->second.index()) return BoundResult::AlreadyBound;

		iter->second = constant;
		++m_BoundCount;
		return BoundResult::Success;
	}
}