#include "ConsoleTreeCommand.h"

bool ConsoleTreeCommand::processCommandLine(vector<string> line)  {
	if (line[0] != "tree") {
		return false;
	}

	if (line.size() == 2) {
		if (line[1] == "rld") {
			LindenmayerTree *selected = (LindenmayerTree*)Context::getMouseManager()->getSelectedGameObject();
			if (selected != nullptr) {
				vec3 position = ((Transform*)selected->getComponent(TRANSFORM))->getPosition();
				GameObject *go = LindenmayerTreeFactory::getInstance().generateTree(selected->getTreeParams(), selected->name, selected->getBarkMaterial(), selected->getLeavesMaterial(), position);
				Context::getSceneManager()->getActiveScene()->removeGameObject(selected);
				Context::getMouseManager()->setSelectedGameObject(go);
			}
		}
	} else if (line.size() % 2 == 1) {
		map<string, string> params = ConsoleUtils::generateParamsMap(line);
		vec3 pos(0, 0, 0);
		string name = "Tree";
		string filename = "LindenmayerRules/randomTree.json";
		string barkTexture = "Textures/barkTexture3.jpg";
		string leavesTexture = "Textures/leaves3.png";

		while (!params.empty()) {
			if (params.find("pos") != params.end()) {
				pos = ConsoleUtils::parseCoords(params["pos"]);
				params.erase("pos");
				continue;
			}
			if (params.find("file") != params.end()) {
				filename = "LindenmayerRules/" + params["file"];
				params.erase("file");
				continue;
			}
			if (params.find("name") != params.end()) {
				name = params["name"];
				params.erase("name");
				continue;
			}
			if (params.find("bark") != params.end()) {
				barkTexture = "Textures/" + params["bark"];
				params.erase("bark");
				continue;
			}
			if (params.find("leaves") != params.end()) {
				leavesTexture = "Textures/" + params["leaves"];
				params.erase("leaves");
				continue;
			}

			ConsoleUtils::logToConsole("Unknown tree parameter!");
		}

		LindenmayerTreeParams lindenmayerParameters = LindenmayerTreeParams(filename);
		LindenmayerTreeFactory::getInstance().generateTree(lindenmayerParameters, name, Material::diffuseTextureOnly(barkTexture), Material::diffuseTextureOnly(leavesTexture), pos);
	}  else {
		ConsoleUtils::logToConsole("Command parameters does not match expected values");
	}
	return true;
}

void ConsoleTreeCommand::printHelpText() {
	ConsoleUtils::logToConsole(" - tree rld");
	ConsoleUtils::logToConsole("    reloads selected tree");
	ConsoleUtils::logToConsole(" - tree pos <vec3> file <string> name <string> bark <string> leaves <string>");
	ConsoleUtils::logToConsole("    generate tree");
}