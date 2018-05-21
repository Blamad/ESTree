#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#include <string>
#include <vector>
#include <map>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <glm/glm.hpp>
#include "Context.h"
#include "ConsoleMemory.h"

using namespace std;
using namespace glm;

class ConsoleUtils {
public:
	static void logToConsole(string val) {
		Context::getConsoleMemory()->pushLine(val);
	}

	static vector<string> split(string &value, string by) {
		vector<string> line;
		boost::split(line, value, boost::is_any_of(by));
		return line;
	}

	static vec3 parseCoords(string &line) {
		vector<string> position = split(line, ",");
		return vec3(strToInt(position[0]), strToInt(position[1]), strToInt(position[2]));
	}

	static int strToInt(string &value) {
		return stoi(value);
	}

	static map<string, string> generateParamsMap(vector<string> &input) {
		map<string, string> params;
		for (int i = 1; i < input.size(); i += 2) {
			params[input[i]] = input[i + 1];
		}
		return params;
	}
};

#endif