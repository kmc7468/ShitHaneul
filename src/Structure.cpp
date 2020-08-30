#include <ShitHaneul/Structure.hpp>

#include <utility>

namespace ShitHaneul {
	Structure::Structure(const JosaList& fieldList)
		: FieldList(fieldList) {}
	Structure::Structure(const Structure& structure)
		: FieldList(structure.FieldList) {}
	Structure::Structure(Structure&& structure) noexcept
		: FieldList(std::move(structure.FieldList)) {}

	Structure& Structure::operator=(const Structure& structure) {
		FieldList = structure.FieldList;
		return *this;
	}
	Structure& Structure::operator=(Structure&& structure) noexcept {
		FieldList = std::move(structure.FieldList);
		return *this;
	}
}