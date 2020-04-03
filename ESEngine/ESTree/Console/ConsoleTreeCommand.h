#ifndef CONSOLE_TREE_COMMAND_H
#define CONSOLE_TREE_COMMAND_H

#include "Engine/UI/ConsoleCustomCommand.h"
#include "Engine/Manager/MouseManager.h"
#include "ESTree/Models/Tree/LindenmayerTreeFactory.h"
#include "Engine/Manager/SceneManager.h"
#include "Engine/UI/ConsoleUtils.h"

class ConsoleTreeCommand : public ConsoleCustomCommand {
public:
	ConsoleTreeCommand() : ConsoleCustomCommand() {};

	bool processCommandLine(std::vector<std::string> commandLine) override;
	void printHelpText() override;
};

#endif