#pragma once
#include "../Memory.hpp"

#include <algorithm>

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