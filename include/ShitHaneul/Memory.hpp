#pragma once

#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Function.hpp>

#include <cstddef>
#include <type_traits>
#include <variant>
#include <vector>

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

namespace ShitHaneul {
	using ManagedConstant = std::variant<std::monostate, Function, StringMap>;

	class Page final {
	private:
		std::vector<ManagedConstant> m_Page;
		std::size_t m_Used = 0;

	public:
		explicit Page(std::size_t count);
		Page(Page&& page) noexcept;
		~Page() = default;

	public:
		Page& operator=(Page&& page) noexcept;
		const ManagedConstant& operator[](std::size_t index) const noexcept;
		ManagedConstant& operator[](std::size_t index) noexcept;

	public:
		std::size_t GetSize() const noexcept;
		std::size_t GetUsedSize() const noexcept;
		std::size_t GetUnusedSize() const noexcept;

		template<typename... Args>
		Function* CreateFunction(Args&&... args) noexcept(std::is_nothrow_constructible_v<Function, Args...>);
		template<typename... Args>
		StringMap* CreateStructure(Args&&... args) noexcept(std::is_nothrow_constructible_v<StringMap, Args...>);
	};
}

#include "impl/Memory.hpp"