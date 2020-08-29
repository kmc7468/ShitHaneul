#pragma once

#include <ShitHaneul/Constant.hpp>

#include <cstdint>
#include <vector>

namespace ShitHaneul {
	class ConstantList final {
	private:
		std::vector<Constant> m_List;

	public:
		ConstantList() noexcept = default;
		ConstantList(ConstantList&& constantList) noexcept;
		~ConstantList() = default;

	public:
		ConstantList& operator=(ConstantList&& constantList) noexcept;
		Constant operator[](std::uint64_t index) const noexcept;

	public:
		template<typename T>
		void AddConstant(T rawConstant);
		std::uint64_t GetCount() const noexcept;
		void ReserveConstants(std::uint64_t count);
	};
}

#include "impl/ConstantList.hpp"