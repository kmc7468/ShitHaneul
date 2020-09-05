#include <ShitHaneul/Interpreter.hpp>
#include <ShitHaneul/Parser.hpp>

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <locale>
#include <optional>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) {
	using namespace ShitHaneul;

	std::locale::global(std::locale(""));

	if (argc < 2) {
		WriteStringToStdout(u8"파일이 필요합니다.");
		return EXIT_FAILURE;
	}

	Parser parser;
	if (!parser.Load(argv[1])) {
		WriteStringToStdout(u8"파일을 읽을 수 없습니다.");
		return EXIT_FAILURE;
	}
	parser.Parse();

	Interpreter interpreter;
	interpreter.Load(parser.GetResult());
	if (!interpreter.Interpret()) {
		const auto& stackTrace = interpreter.GetStackTrace();
		for (auto iter = stackTrace.begin(); iter < stackTrace.end(); ++iter) {
			const StackFrame& frame = *iter;
			const FunctionInfo* func = frame.GetCurrentFunction()->Info;
			const LineInfo line = func->LineMap[frame.GetCurrentOffset()];

			std::ostringstream oss;
			oss << u8"파일 '" << EncodeUTF32ToUTF8(line.Path) << u8"'의 " << line.Line << u8"번째 줄";
			if (func->Name.size()) {
				oss << u8", 함수 '" << EncodeUTF32ToUTF8(func->Name) << '\'';
			}
			oss << u8":\n";
			WriteStringToStdout(oss.str());
		}

		const auto& exception = interpreter.GetException();
		std::ifstream errorFile(EncodeUTF32ToUTF8(exception.Line.Path));
		if (errorFile) {
			const std::string line = std::to_string(exception.Line.Line);
			std::string topBottom(line.size() + 2, ' ');
			topBottom += u8"|\n";
			std::string code;
			for (std::uint16_t i = 0; i < exception.Line.Line; ++i) {
				std::getline(errorFile, code);
			}
			code.push_back('\n');

			WriteStringToStdout(topBottom);
			WriteStringToStdout(' ' + line + u8" | " + code);
			WriteStringToStdout(topBottom);
		}

		WriteStringToStdout(u8"\n오류: ");
		WriteStringToStdout(exception.Mesasge);
		WriteCharacterToStdout('\n');

		return EXIT_FAILURE;
	} else return EXIT_SUCCESS;
}