#pragma once
#include "../Parser.hpp"

#include <ShitHaneul/Memory.hpp>

namespace ShitHaneul {
	template<typename T>
	T Parser::ReadScalar() {
		T result = *reinterpret_cast<T*>(&m_Bytes[m_Cursor]);
		m_Cursor += sizeof(result);

		if (sizeof(result) > 1 && GetEndian() != Endian::Big) {
			result = ReverseEndian(result);
		}
		return result;
	}
	template<typename S>
	std::u32string Parser::ReadString() {
		const auto length = ReadScalar<S>();
		std::u32string result(static_cast<std::size_t>(length), 0);

		for (S i = 0; i < length; ++i) {
			result[static_cast<std::size_t>(i)] = ReadCharacter();
		}
		return result;
	}
}