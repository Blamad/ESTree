#include "ConsoleInterpreter.h"

#include "UIManager.h"
#include "SceneManager.h"

Logger ConsoleInterpreter::logger("ConsoleInterpreter");
vector<shared_ptr<ConsoleCustomCommand>> ConsoleInterpreter::customConsoleCommands;

void ConsoleInterpreter::addCustomCommand(shared_ptr<ConsoleCustomCommand> consoleCommand) {
	customConsoleCommands.push_back(consoleCommand);
}

void ConsoleInterpreter::processInput(string &input) {
	try {
		vector<string> line = ConsoleUtils::split(input, " ");

		if (line[0] == "help") {
			displayHelp();
			return;
		}

		if (line[0] == "depth" && line.size() == 2) {
			if (line[1] == "off") {
				Context::getUIManager()->toggleDepthBufferComponent(false);
			}
			if (line[1] == "on") {
				Context::getUIManager()->toggleDepthBufferComponent(true);
			}
			return;
		}

		if (line[0] == "rm") {
			GameObject *selected = Context::getMouseManager()->getSelectedGameObject();
			if (selected != nullptr) {
				string name = selected->name;
				Context::getSceneManager()->getActiveScene()->removeGameObject(selected);
				ConsoleUtils::logToConsole("'" + name + "' removed");
			}
			return;
		}

		if (line[0] == "mv" && line.size() == 2) {
			GameObject *selected = Context::getMouseManager()->getSelectedGameObject();
			if (selected != nullptr) {
				vec3 position = ConsoleUtils::parseCoords(line[1]);
				RigidBody *rigidbody = (RigidBody*)selected->getComponent(RIGIDBODY);
				rigidbody->translate(position);
			}
			return;
		}

		BOOST_FOREACH(auto& command, customConsoleCommands) {
			if (command->processCommandLine(line)) {
				return;
			}
		}

		ConsoleUtils::logToConsole("Unknown command '"+line[0]+"'");
	}
	catch (exception e) {
		ConsoleUtils::logToConsole("Something went wrong!");
		logger.log(ERROR, e.what());
	}
}

void ConsoleInterpreter::displayHelp() {
	ConsoleUtils::logToConsole("Available commands:");
	ConsoleUtils::logToConsole(" - depth <on/off>");
	ConsoleUtils::logToConsole("    display depth buffer");
	ConsoleUtils::logToConsole(" - rm");
	ConsoleUtils::logToConsole("    removes selected object");
	ConsoleUtils::logToConsole(" - mv <vec3>");
	ConsoleUtils::logToConsole("    moves selected object by given vector");

	BOOST_FOREACH(auto& command, customConsoleCommands) {
		command->printHelpText();
	}
}