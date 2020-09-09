#pragma once
#include "../Memory.hpp"

#include <algorithm>
#include <cstdint>

namespace ShitHaneul {
#ifdef SHN_LITTLE
	constexpr Endian GetEndian() noexcept {
		return Endian::Little;
	}
#endif

	template<typename T>
	T ReverseEndian(const T& value) noexcept {
		union Transformer {
			std::uint8_t Bytes[sizeof(value)];
		} temp;
		temp = reinterpret_cast<const Transformer&>(value);
		std::reverse(temp.Bytes, temp.Bytes + sizeof(value));
		return reinterpret_cast<T&>(temp);
	}
}

namespace ShitHaneul {
	template<typename... Args>
	Function* Page::CreateFunction(Args&&... args) noexcept(std::is_nothrow_constructible_v<Function, Args...>) {
		return &std::get<Function>(m_Page[m_Used++] = Function(std::forward<Args>(args)...));
	}
	template<typename... Args>
	StringMap* Page::CreateStructure(Args && ...args) noexcept(std::is_nothrow_constructible_v<StringMap, Args...>) {
		return &std::get<StringMap>(m_Page[m_Used++] = StringMap(std::forward<Args>(args)...));
	}
}