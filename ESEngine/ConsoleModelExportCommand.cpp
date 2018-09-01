#include "ConsoleModelExportCommand.h"

Logger ConsoleModelExportCommand::logger("ConsoleModelExportCommand");

bool ConsoleModelExportCommand::processCommandLine(vector<string> line) {
	if (line[0] == "export") {

		map<string, string> params = ConsoleUtils::generateParamsMap(line);
		string filename = "esTreeModel";

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
			logger.log(INFO, "Exporting of " + selected->name + " started..");
			vector<Component*> components = (vector<Component*>)selected->getComponents(RENDERABLE);
			vector<Renderable*> renderables;
			for (auto & component : components) 
				renderables.push_back((Renderable*) component);
			for (auto & child : selected->children) {
				components = (vector<Component*>)child->getComponents(RENDERABLE);
				for (auto & component : components)
					renderables.push_back((Renderable*)component);
			}
			ModelExporter::exportToFile(renderables, filename);
			ConsoleUtils::logToConsole(selected->name + " exported to file");
			logger.log(INFO, "Exporting of " + selected->name + " finished.");
		}
		return true;
	}
	return false;
}

void ConsoleModelExportCommand::printHelpText() {
	ConsoleUtils::logToConsole(" - export file <string>");
	ConsoleUtils::logToConsole("    export model to file");
}