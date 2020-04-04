#ifndef CONSOLE_INTERPRETER_H
#define CONSOLE_INTERPRETER_H

#include <string>
#include <vector>
#include <boost/foreach.hpp>

#include "Engine/Manager/Context.h"
#include "ConsoleMemory.h"
#include "ConsoleCustomCommand.h"
#include "ConsoleUtils.h"

#include "Engine/Core/Logger.h"

class ConsoleInterpreter {
public:
	void processInput(std::string &input);
	static void addCustomCommand(std::shared_ptr<ConsoleCustomCommand> consoleCommand);
private:
	static Logger logger;
	static std::vector<std::shared_ptr<ConsoleCustomCommand>> customConsoleCommands;

	static void displayHelp();
};

#endif