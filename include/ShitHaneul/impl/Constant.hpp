#pragma once
#include "../Constant.hpp"

namespace ShitHaneul {
	template<typename T>
	void ConstantList::Add(T rawConstant) {
		using ConstantClass = MakeConstantClass<T>;
		static_assert(!std::is_same_v<ConstantClass, void>);
		m_List.push_back(ConstantClass(rawConstant));
	}
}