#ifndef CONSOLE_EXPORT_COMMAND_H
#define CONSOLE_EXPORT_COMMAND_H

#include "ConsoleCustomCommand.h"
#include "MouseManager.h"
#include "ConsoleUtils.h"
#include "ModelExporter.h"
#include "Logger.h"

class ConsoleModelExportCommand : public ConsoleCustomCommand {
public:
	ConsoleModelExportCommand() : ConsoleCustomCommand() {};

	bool processCommandLine(vector<string> commandLine) override;
	void printHelpText() override;
private:
	static Logger logger;
};

#endif
