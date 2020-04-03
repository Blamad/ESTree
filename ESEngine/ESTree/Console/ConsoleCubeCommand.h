#ifndef CONSOLE_CUBE_COMMAND_H
#define CONSOLE_CUBE_COMMAND_H

#include "Engine/UI/ConsoleCustomCommand.h"
#include "Engine/Manager/MouseManager.h"
#include "Engine/Manager/SceneManager.h"
#include "Engine/UI/ConsoleUtils.h"
#include "ESTree/Models/Cube.h"
#include "ESTree/Models/LampCube.h"
#include "Engine/GameObject/PointLight.h"

class ConsoleCubeCommand : public ConsoleCustomCommand {
public:
	ConsoleCubeCommand() : ConsoleCustomCommand() {};

	bool processCommandLine(std::vector<std::string> commandLine) override;
	void printHelpText() override;
};

#endif