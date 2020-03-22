#ifndef CONSOLE_CUBE_COMMAND_H
#define CONSOLE_CUBE_COMMAND_H

#include "ConsoleCustomCommand.h"
#include "MouseManager.h"
#include "SceneManager.h"
#include "ConsoleUtils.h"
#include "Cube.h"
#include "LampCube.h"
#include "PointLight.h"

class ConsoleCubeCommand : public ConsoleCustomCommand {
public:
	ConsoleCubeCommand() : ConsoleCustomCommand() {};

	bool processCommandLine(std::vector<std::string> commandLine) override;
	void printHelpText() override;
};

#endif