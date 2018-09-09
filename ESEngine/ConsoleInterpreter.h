#ifndef CONSOLE_INTERPRETER_H
#define CONSOLE_INTERPRETER_H

#include <string>
#include <vector>
#include <boost/foreach.hpp>

#include "Context.h"
#include "ConsoleMemory.h"
#include "ConsoleCustomCommand.h"
#include "ConsoleUtils.h"

#include "Logger.h"

using namespace std;

class ConsoleInterpreter {
public:
	void processInput(string &input);
	static void addCustomCommand(shared_ptr<ConsoleCustomCommand> consoleCommand);
private:
	static Logger logger;
	static vector<shared_ptr<ConsoleCustomCommand>> customConsoleCommands;

	static void displayHelp();
};

#endif