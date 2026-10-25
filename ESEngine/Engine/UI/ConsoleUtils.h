#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "Engine/Manager/Context.h"
#include "ConsoleMemory.h"

class ConsoleUtils {
public:
	static void logToConsole(std::string val) {
		Context::getConsoleMemory()->pushLine(val);
	}

	static std::vector<std::string> split(std::string &value, std::string by) {
		// Splits on every character contained in `by`; empty tokens are kept.
		std::vector<std::string> line;
		size_t start = 0;
		size_t pos;
		while ((pos = value.find_first_of(by, start)) != std::string::npos) {
			line.push_back(value.substr(start, pos - start));
			start = pos + 1;
		}
		line.push_back(value.substr(start));
		return line;
	}

	static glm::vec3 parseCoords(std::string &line) {
		std::vector<std::string> position = split(line, ",");
		return glm::vec3(strToInt(position.at(0)), strToInt(position.at(1)), strToInt(position.at(2)));
	}

	static int strToInt(std::string &value) {
		return stoi(value);
	}

	static std::map<std::string, std::string> generateParamsMap(std::vector<std::string> &input) {
		std::map<std::string, std::string> params;
		for (int i = 1; i < input.size(); i += 2) {
			params[input.at(i)] = input.at(i + 1);
		}
		return params;
	}
};

#endif