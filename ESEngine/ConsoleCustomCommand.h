#ifndef CUSTOM_CONSOLE_COMMAND_H
#define CUSTOM_CONSOLE_COMMAND_H

#include "Context.h"
#include "ConsoleMemory.h"

class ConsoleCustomCommand {
public:
	ConsoleCustomCommand() {}

	virtual bool processCommandLine(vector<string> commandLine) = 0;
	virtual void printHelpText() = 0;
};

#endif