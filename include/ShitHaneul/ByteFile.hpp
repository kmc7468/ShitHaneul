#pragma once

#include <ShitHaneul/Function.hpp>

#include <vector>

namespace ShitHaneul {
	class ByteFile final {
	private:
		std::vector<FunctionInfo*> m_FunctionInfos;
		std::vector<Function*> m_Functions;
		Function* m_RootFunction = nullptr;

	public:
		ByteFile() noexcept = default;
		ByteFile(ByteFile&& byteFile) noexcept;
		~ByteFile();

	public:
		ByteFile& operator=(ByteFile&& byteFile) noexcept;

	public:
		void Clear() noexcept;

		Function* RegisterFunction(FunctionInfo* functionInfo);
		void AddFunction(Function* function);
		void SetRoot(Function* function);
	};
}