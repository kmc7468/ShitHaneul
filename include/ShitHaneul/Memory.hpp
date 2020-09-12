#pragma once

#include <ShitHaneul/Constant.hpp>
#include <ShitHaneul/Function.hpp>

#include <cstddef>
#include <cstdint>
#include <list>
#include <type_traits>
#include <utility>
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
	enum class MarkedColor {
		White,
		Gray,
		Black,
	};

	struct ManagedConstant final {
		std::variant<std::monostate, Function, StringMap> Value;
		std::uint64_t Age = 0;
		MarkedColor Color = MarkedColor::White;
	};
}

namespace ShitHaneul {
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
		bool IsEmpty() const noexcept;
		bool IsFull() const noexcept;
		std::size_t GetSize() const noexcept;
		std::size_t GetUsedSize() const noexcept;
		std::size_t GetUnusedSize() const noexcept;

		ManagedConstant* Allocate() noexcept;
	};
}

namespace ShitHaneul {
	class Generation final {
	public:
		using PageIterator = std::list<Page>::iterator;

	private:
		std::list<Page> m_Pages;
		PageIterator m_CurrentPage;
		std::size_t m_PageSize;

	public:
		explicit Generation(std::size_t pageSize);
		Generation(Generation&& generation) noexcept;
		~Generation() = default;

	public:
		Generation& operator=(Generation&& generation) noexcept;

	public:
		void Reset() noexcept;
		void Initialize(std::size_t pageSize);

		PageIterator CreatePage();
		PageIterator GetCurrentPage() noexcept;

		ManagedConstant* Allocate() noexcept;
	};
}

namespace ShitHaneul {
	class ByteFile;

	class GarbageCollector final {
	private:
		Generation m_YoungGeneration;
		Generation m_OldGeneration;

	public:
		GarbageCollector(std::size_t youngPageSize, std::size_t oldPageSize);
		GarbageCollector(GarbageCollector&& garbageCollector) noexcept;
		~GarbageCollector() = default;

	public:
		GarbageCollector& operator=(GarbageCollector&& garbageCollector) noexcept;

	public:
		void Reset() noexcept;
		void Initialize(std::size_t youngPageSize, std::size_t oldPageSize);
	};
}

#include "impl/Memory.hpp"