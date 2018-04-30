#ifndef CONSOLE_INTERPRETER_H
#define CONSOLE_INTERPRETER_H

#include <string>
#include <vector>
#include <map>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include "Context.h"

#include "LindenmayerTreeParams.h"
#include "LindenmayerTree.h"
#include "Cube.h"

using namespace std;

class ConsoleInterpreter {
public:
	static void processInput(string &input);
private:
	static vector<string> split(string &value, string by) {
		vector<string> line;
		boost::split(line, value, boost::is_any_of(by));
		return line;
	}

	static void processTreeCommand(map<string, string> params);

	static GameObject* createTree(LindenmayerTreeParams &params, string &name, Material &barkTexture, Material &leavesTexture, vec3 &position);
	
	static vec3 parseCoords(string line) {
		vector<string> position = split(line, ",");
		return vec3(strToInt(position[0]), strToInt(position[1]), strToInt(position[2]));
	}

	static void logToConsole(string val) {
		//Context::getConsoleComponent()->writeLine(val);
	}

	static int strToInt(string &value) {
		return stoi(value);
	}
};

#endif