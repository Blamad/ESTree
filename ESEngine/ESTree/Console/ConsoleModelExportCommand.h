#ifndef CONSOLE_EXPORT_COMMAND_H
#define CONSOLE_EXPORT_COMMAND_H

#include "Engine/UI/ConsoleCustomCommand.h"
#include "Engine/Manager/MouseManager.h"
#include "Engine/UI/ConsoleUtils.h"
#include "ESTree/Models/ModelExporter.h"
#include "Engine/Core/Logger.h"

class ConsoleModelExportCommand : public ConsoleCustomCommand {
public:
	ConsoleModelExportCommand() : ConsoleCustomCommand() {};

	bool processCommandLine(std::vector<std::string> commandLine) override;
	void printHelpText() override;
private:
	static Logger logger;
};

#endif
