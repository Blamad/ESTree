#include "ConsoleInterpreter.h"

#include "Engine/Manager/UIManager.h"
#include "Engine/Manager/SceneManager.h"

Logger ConsoleInterpreter::logger("ConsoleInterpreter");
std::vector<std::shared_ptr<ConsoleCustomCommand>> ConsoleInterpreter::customConsoleCommands;

void ConsoleInterpreter::addCustomCommand(std::shared_ptr<ConsoleCustomCommand> consoleCommand) {
	customConsoleCommands.push_back(consoleCommand);
}

void ConsoleInterpreter::processInput(std::string &input) {
	try {
		std::vector<std::string> line = ConsoleUtils::split(input, " ");

		if (line.at(0) == "help") {
			displayHelp();
			return;
		}

		if (line.at(0) == "normals" && line.size() == 2) {
			GameObject *selected = Context::getMouseManager()->getSelectedGameObject();
			if (selected != nullptr) {
				Mesh *mesh = (Mesh*)selected->getComponent(RENDERABLE);
				if (line.at(1) == "off") {
					mesh->hideNormalVisualisation();
				}
				if (line.at(1) == "on") {
					mesh->showNormalVisualisation();
				}
			}
			
			return;
		}

		if (line.at(0) == "wiring" && line.size() == 2) {
			GameObject *selected = Context::getMouseManager()->getSelectedGameObject();
			if (selected != nullptr) {
				Mesh *mesh = (Mesh*)selected->getComponent(RENDERABLE);
				if (line.at(1) == "off") {
					mesh->hideMeshWiring();
				}
				if (line.at(1) == "on") {
					mesh->showMeshWiring();
				}
			}

			return;
		}

		if (line.at(0) == "depth" && line.size() == 2) {
			if (line.at(1) == "off") {
				Context::getUIManager()->toggleDepthBufferComponent(false);
			}
			if (line.at(1) == "on") {
				Context::getUIManager()->toggleDepthBufferComponent(true);
			}
			return;
		}

		if (line.at(0) == "rm") {
			GameObject *selected = Context::getMouseManager()->getSelectedGameObject();
			if (selected != nullptr) {
				std::string name = selected->name;
				Context::getSceneManager()->getActiveScene()->removeGameObject(selected);
				ConsoleUtils::logToConsole("'" + name + "' removed");
			}
			return;
		}

		if (line.at(0) == "clr") {
			Context::getConsoleMemory()->clear();
			return;
		}

		if (line.at(0) == "hdr" && line.size() == 2) {
			if (line.at(1) == "on") {
				Context::setHdrToggle(true);
				return;
			}
			if (line.at(1) == "off") {
				Context::setHdrToggle(false);
				return;
			}
		}

		if (line.at(0) == "mv" && line.size() == 2) {
			GameObject *selected = Context::getMouseManager()->getSelectedGameObject();
			if (selected != nullptr) {
				glm::vec3 position = ConsoleUtils::parseCoords(line.at(1));
				RigidBody *rigidbody = (RigidBody*)selected->getComponent(RIGIDBODY);
				rigidbody->translate(position);
			}
			return;
		}

		if (line.at(0) == "fps") {
			boost::posix_time::ptime startTime = Context::getStartTime();
			boost::posix_time::ptime endTime = boost::posix_time::microsec_clock::local_time();
			long frames = Context::getFrameCounterValue();

			int milliseconds = (endTime - startTime).total_milliseconds();
			float fps = float(frames) * 1000/ milliseconds;
			ConsoleUtils::logToConsole("Current fps: '" + std::to_string(fps) + "'");

			if (line.size() == 2) {
				if (line.at(1) == "clr") {
					Context::clearFrameCounter();
					Context::setStartTime();
				}
			}

			return;
		}

		BOOST_FOREACH(auto& command, customConsoleCommands) {
			if (command->processCommandLine(line)) {
				return;
			}
		}

		ConsoleUtils::logToConsole("Unknown command or wrong args: '"+ input +"'");
	}
	catch (std::exception& e) {
		ConsoleUtils::logToConsole("Something went wrong!");
		logger.log(LOG_ERROR, e.what());
	}
	catch (...) {
		ConsoleUtils::logToConsole("Something went wrong!");
		logger.log(LOG_ERROR, "Unknown exception while parsing command");
	}
}

void ConsoleInterpreter::displayHelp() {
	ConsoleUtils::logToConsole("Available commands:");
	ConsoleUtils::logToConsole(" - depth <on/off>");
	ConsoleUtils::logToConsole("    display depth buffer");
	ConsoleUtils::logToConsole(" - hdr <on/off>");
	ConsoleUtils::logToConsole("    toggle hdr on/off");
	ConsoleUtils::logToConsole(" - normals <on/off>");
	ConsoleUtils::logToConsole("    toggle mesh normals on/off");
	ConsoleUtils::logToConsole(" - wiring <on/off>");
	ConsoleUtils::logToConsole("    toggle mesh wiring on/off");
	ConsoleUtils::logToConsole(" - clr");
	ConsoleUtils::logToConsole("    clear console");
	ConsoleUtils::logToConsole(" - rm");
	ConsoleUtils::logToConsole("    remove selected object");
	ConsoleUtils::logToConsole(" - mv <vec3>");
	ConsoleUtils::logToConsole("    translate selected object by given vector");

	BOOST_FOREACH(auto& command, customConsoleCommands) {
		command->printHelpText();
	}
}