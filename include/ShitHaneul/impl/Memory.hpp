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
		return &std::get<Function>((m_Page[m_Used++] = std::make_pair(Function(std::forward<Args>(args)...), 0)).first);
	}
	template<typename... Args>
	StringMap* Page::CreateStructure(Args && ...args) noexcept(std::is_nothrow_constructible_v<StringMap, Args...>) {
		return &std::get<StringMap>((m_Page[m_Used++] = std::make_pair(StringMap(std::forward<Args>(args)...), 0)).first);
	}
}

namespace ShitHaneul {
	template<typename... Args>
	Function* Generation::CreateFunction(Args&&... args) noexcept(std::is_nothrow_constructible_v<Function, Args...>) {
		if (m_CurrentPage->IsFull()) return nullptr;
		else return m_CurrentPage->CreateFunction(std::forward<Args>(args)...);
	}
	template<typename... Args>
	StringMap* Generation::CreateStructure(Args&&... args) noexcept(std::is_nothrow_constructible_v<StringMap, Args...>) {
		if (m_CurrentPage->IsFull()) return nullptr;
		else return m_CurrentPage->CreateStructure(std::forward<Args>(args)...);
	}
}