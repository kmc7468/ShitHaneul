#pragma once

#ifdef _WIN32
#	define SHN_WINDOWS
#endif

#if defined(_M_IX86) || defined(__i386) || defined(_X86_) || defined(__X86__) || defined(__THW_INTEL__) || defined(__I86__) || defined(__INTEL__) || defined(__386)
#	define SHN_X86
#endif

#if defined(_M_X64) || defined(__amd64)
#	ifndef SHN_X86
#		define SHN_X86
#	endif
#	define SHN_X64
#endif

#if defined(SHN_WINDOWS) || defined(SHN_X86)
#	define SHN_LITTLE
#endif

namespace ShitHaneul {
	enum class Endian {
		Little,
		Big,
	};

#ifdef SHN_LITTLE
	constexpr Endian GetEndian() noexcept;
#else
	Endian GetEndian() noexcept;
#endif

	template<typename T>
	T ReverseEndian(const T& value) noexcept;
}

#include "impl/Memory.hpp"