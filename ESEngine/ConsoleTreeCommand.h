#ifndef CONSOLE_TREE_COMMAND_H
#define CONSOLE_TREE_COMMAND_H

#include "ConsoleCustomCommand.h"
#include "MouseManager.h"
#include "LindenmayerTreeFactory.h"
#include "SceneManager.h"
#include "ConsoleUtils.h"

class ConsoleTreeCommand : public ConsoleCustomCommand {
public:
	ConsoleTreeCommand() : ConsoleCustomCommand() {};

	bool processCommandLine(vector<string> commandLine) override;
	void printHelpText() override;
};

#endif