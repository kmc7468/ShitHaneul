#pragma once
#include "../ConstantList.hpp"

#include <type_traits>

namespace ShitHaneul {
	template<typename T>
	void ConstantList::AddConstant(T rawConstant) {
		using ConstantClass = MakeConstantClass<T>;
		static_assert(!std::is_same_v<ConstantClass, void>);
		m_List.push_back(ConstantClass(rawConstant));
	}
}