#ifndef CUSTOM_CONSOLE_COMMAND_H
#define CUSTOM_CONSOLE_COMMAND_H

#include "Engine/Manager/Context.h"
#include "ConsoleMemory.h"

class ConsoleCustomCommand {
public:
	ConsoleCustomCommand() {}

	virtual bool processCommandLine(std::vector<std::string> commandLine) = 0;
	virtual void printHelpText() = 0;
};

#endif