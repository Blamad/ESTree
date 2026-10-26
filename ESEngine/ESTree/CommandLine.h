#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

// Header-only command line parser for ESTree.exe. Depends on the standard library only.
namespace CommandLine {

constexpr int DEFAULT_WIDTH = 1366;
constexpr int DEFAULT_HEIGHT = 768;
constexpr int DEFAULT_SCENE = 0;
constexpr int MAX_SCENE = 4;

struct Options {
	int width = DEFAULT_WIDTH;
	int height = DEFAULT_HEIGHT;
	int scene = DEFAULT_SCENE;
	bool fullscreen = false;
	bool help = false;
};

struct Result {
	bool ok = true;
	std::string error;
	Options options;
};

inline std::string helpText() {
	return
		"Allowed options:\n"
		"  -h [ --help ]              produce help message\n"
		"  -W [ --width ] arg (=1366) set window width\n"
		"  -H [ --height ] arg (=768) set window height\n"
		"  -s [ --scene ] arg (=0)    load one of five scenes (0-4):\n"
		"                               0 - single tree\n"
		"                               1 - physics\n"
		"                               2 - forest\n"
		"                               3 - test\n"
		"                               4 - tree\n"
		"  -f [ --fullscreen ]        launch application in fullscreen mode\n";
}

namespace detail {

inline bool parseInt(const std::string &text, int &out) {
	try {
		size_t used = 0;
		int value = std::stoi(text, &used);
		if (used != text.size())
			return false;
		out = value;
		return true;
	}
	catch (const std::exception &) {
		return false;
	}
}

inline Result fail(const std::string &message) {
	Result result;
	result.ok = false;
	result.error = message;
	return result;
}

} // namespace detail

// argv[0] is the program name and is skipped. Never throws.
inline Result parse(int argc, const char *const *argv) {
	Result result;
	Options &options = result.options;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		std::string name;
		std::string value;
		bool hasValue = false;

		if (arg.size() > 2 && arg.compare(0, 2, "--") == 0) {
			size_t eq = arg.find('=');
			name = arg.substr(2, eq == std::string::npos ? std::string::npos : eq - 2);
			if (eq != std::string::npos) {
				value = arg.substr(eq + 1);
				hasValue = true;
			}
		}
		else if (arg.size() >= 2 && arg[0] == '-' && arg[1] != '-') {
			// Short option, optionally with the value glued on (-W800).
			name = std::string(1, arg[1]);
			if (arg.size() > 2) {
				value = arg.substr(2);
				hasValue = true;
			}
		}
		else {
			return detail::fail("unrecognised option '" + arg + "'");
		}

		if (name == "help" || name == "h") {
			if (hasValue)
				return detail::fail("option '" + arg + "' does not take any arguments");
			options.help = true;
		}
		else if (name == "fullscreen" || name == "f") {
			if (hasValue)
				return detail::fail("option '" + arg + "' does not take any arguments");
			options.fullscreen = true;
		}
		else if (name == "width" || name == "W" || name == "height" || name == "H" || name == "scene" || name == "s") {
			std::string longName = (name == "W" ? "width" : name == "H" ? "height" : name == "s" ? "scene" : name);

			if (!hasValue) {
				if (i + 1 >= argc)
					return detail::fail("the required argument for option '--" + longName + "' is missing");
				value = argv[++i];
			}

			int number;
			if (!detail::parseInt(value, number))
				return detail::fail("the argument ('" + value + "') for option '--" + longName + "' is invalid");

			if (longName == "scene") {
				if (number < 0 || number > MAX_SCENE)
					return detail::fail("the argument ('" + value + "') for option '--scene' is out of range (0-" + std::to_string(MAX_SCENE) + ")");
				options.scene = number;
			}
			else {
				if (number <= 0)
					return detail::fail("the argument ('" + value + "') for option '--" + longName + "' must be positive");
				(longName == "width" ? options.width : options.height) = number;
			}
		}
		else {
			return detail::fail("unrecognised option '" + arg + "'");
		}
	}

	return result;
}

} // namespace CommandLine
