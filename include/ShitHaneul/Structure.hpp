#pragma once

#include <ShitHaneul/Josa.hpp>

namespace ShitHaneul {
	class Structure final {
	public:
		JosaMap FieldList;

	public:
		Structure() noexcept = default;
		explicit Structure(const JosaList& fieldList);
		Structure(const Structure& structure);
		Structure(Structure&& structure) noexcept;
		~Structure() = default;

	public:
		Structure& operator=(const Structure& structure);
		Structure& operator=(Structure&& structure) noexcept;
	};
}