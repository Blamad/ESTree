#include "ConsoleModelExportCommand.h"

Logger ConsoleModelExportCommand::logger("ConsoleModelExportCommand");

bool ConsoleModelExportCommand::processCommandLine(std::vector<std::string> line) {
	if (line.at(0) == "export") {

		std::map<std::string, std::string> params = ConsoleUtils::generateParamsMap(line);
		std::string filename = "esTreeModel";

		GameObject *selected = (GameObject*)Context::getMouseManager()->getSelectedGameObject();;

		while (!params.empty()) {
			if (params.find("file") != params.end()) {
				filename = params["file"];
				params.erase("file");
				continue;
			}
			ConsoleUtils::logToConsole("Unknown export parameter!");
			break;
		}

		if (selected != nullptr) {
			logger.log(LOG_INFO, "Exporting of " + selected->name + " started..");
			std::vector<Component*> components = (std::vector<Component*>)selected->getComponents(RENDERABLE);
			std::vector<Renderable*> renderables;
			for (auto & component : components) 
				renderables.push_back((Renderable*) component);
			for (auto & child : selected->children) {
				components = (std::vector<Component*>)child->getComponents(RENDERABLE);
				for (auto & component : components)
					renderables.push_back((Renderable*)component);
			}
			ModelExporter::exportToFile(renderables, filename);
			ConsoleUtils::logToConsole(selected->name + " exported to file");
			logger.log(LOG_INFO, "Exporting of " + selected->name + " finished.");
		}
		return true;
	}
	return false;
}

void ConsoleModelExportCommand::printHelpText() {
	ConsoleUtils::logToConsole(" - export file <string>");
	ConsoleUtils::logToConsole("    export model to file");
}