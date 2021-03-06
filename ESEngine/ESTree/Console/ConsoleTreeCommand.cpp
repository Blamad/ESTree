#include "ConsoleTreeCommand.h"

bool ConsoleTreeCommand::processCommandLine(std::vector<std::string> line)  {
	if (line.at(0) != "rtree" && line.at(0) != "tree") {
		return false;
	}

	LindenmayerTree *selected = nullptr;
	bool rldTree = false;

	glm::vec3 pos(0, 0, 0);
	std::string name = "Tree";
	LindenmayerTreeParams lindenmayerParameters = LindenmayerTreeParams("Resources/LindenmayerRules/randomTree.json");
	Material barkMaterial = Material::diffuseTextureOnly("Resources/Textures/barkTexture3.jpg");
	Material leavesMaterial = Material::diffuseTextureOnly("Resources/Textures/leaves3.png");

	std::map<std::string, std::string> params = ConsoleUtils::generateParamsMap(line);

	if (line.at(0) == "rtree") {
		selected = (LindenmayerTree*) Context::getMouseManager()->getSelectedGameObject();
		if (selected != nullptr) {
			rldTree = true;
			pos = ((Transform*)selected->getComponent(TRANSFORM))->getPosition();
			name = selected->name;
			lindenmayerParameters = selected->getTreeParams();
			barkMaterial = selected->getBarkMaterial();
			leavesMaterial = selected->getLeavesMaterial();
		}
		else {
			ConsoleUtils::logToConsole("No tree selected!");
			return false;
		}
	}

	if (line.size() % 2 == 1) {
		while (!params.empty()) {
			if (params.find("pos") != params.end()) {
				pos = ConsoleUtils::parseCoords(params["pos"]);
				params.erase("pos");
				continue;
			}
			if (params.find("name") != params.end()) {
				name = params["name"];
				params.erase("name");
				continue;
			}
			if (params.find("file") != params.end()) {
				std::string filePath = "";
				if (params["file"].length() > 2) {
					filePath = "Resources/LindenmayerRules/" + params["file"];
				}
				else {
					int index = stoi(params["file"]);
					static std::vector<std::string> treeParams = {
						"randomTree.json",
						"symetricTree.json",
						"fibbonacciTree.json",
						"advancedTree.json",
						"advancedTree2.json",
						//5
						"test.json",
						"parametricMonopodialTreeA.json",
						"parametricMonopodialTreeB.json",
						"parametricMonopodialTreeC.json",
						"parametricMonopodialTreeD.json",
						//10
						"parametricSympodialTreeA.json",
						"parametricSympodialTreeB.json",
						"parametricSympodialTreeC.json",
						"parametricSympodialTreeD.json",
						"parametricTernaryTreeA.json",
						//15
						"parametricTernaryTreeB.json",
						"parametricTernaryTreeC.json",
						"parametricTernaryTreeD.json"
					};
					filePath = "Resources/LindenmayerRules/" + (treeParams.size() > index ? treeParams.at(index) : treeParams.at(0));
				}
				struct stat buffer;
				if (stat(filePath.c_str(), &buffer) != 0) {
					ConsoleUtils::logToConsole("There is no such file: " + filePath);
					return false;
				}
				lindenmayerParameters = LindenmayerTreeParams(filePath);
				params.erase("file");
				continue;
			}
			if (params.find("bark") != params.end()) {
				std::string barkTexPath = "";
				if (params["bark"].length() > 2) {
					barkTexPath = "Resources/Textures/" + params["bark"];
				}
				else {
					int index = stoi(params["bark"]);
					static std::vector<std::string> barkTextures = {
						"barkTexture1.jpg",
						"barkTexture2.jpg",
						"barkTexture3.jpg",
						"barkTexture4.jpg",
						"barkTexture5.jpg",
						"treeTexture1.jpg",
						"treeTexture2.jpg"
					};
					barkTexPath = "Resources/Textures/" + (barkTextures.size() > index ? barkTextures[index] : barkTextures.at(0));
				}
				struct stat buffer;
				if (stat(barkTexPath.c_str(), &buffer) != 0) {
					ConsoleUtils::logToConsole("There is no such file: " + barkTexPath);
					return false;
				}

				barkMaterial = Material::diffuseTextureOnly(barkTexPath);
				params.erase("bark");
				continue;
			}
			if (params.find("leaves") != params.end()) {
				std::string leavesTexPath = "";
				if (params["leaves"].length() > 2) {
					leavesTexPath = "Resources/Textures/" + params["leaves"];
				}
				else {
					int index = stoi(params["leaves"]);
					static std::vector<std::string> leaves = {
						"leaves1.png",
						"leaves2.png",
						"leaves3.png",
						"leaves4.png",
						"leaves5.png",
						"arrow.png"
					};
					leavesTexPath = "Resources/Textures/" + (leaves.size() > index ? leaves[index] : leaves.at(0));
				}
				struct stat buffer;
				if (stat(leavesTexPath.c_str(), &buffer) != 0) {
					ConsoleUtils::logToConsole("There is no such file: " + leavesTexPath);
					return false;
				}

				leavesMaterial = Material::diffuseTextureOnly(leavesTexPath);
				params.erase("leaves");
				continue;
			}

			ConsoleUtils::logToConsole("Unknown tree parameter!");
		}
	}
	else {
		ConsoleUtils::logToConsole("Command parameters does not match expected values");
	}

	GameObject *go = LindenmayerTreeFactory::getInstance().generateTree(lindenmayerParameters, name, barkMaterial, leavesMaterial, pos);
	if (rldTree) {
		Context::getSceneManager()->getActiveScene()->removeGameObject(selected);
	}
	Context::getMouseManager()->setSelectedGameObject(go);

	return true;
}

void ConsoleTreeCommand::printHelpText() {
	ConsoleUtils::logToConsole(" - rtree pos <vec3> file <string/int> name <string> bark <string/int> leaves <string/int>");
	ConsoleUtils::logToConsole("    reloads selected tree");
	ConsoleUtils::logToConsole(" - tree pos <vec3> file <string/int> name <string> bark <string/int> leaves <string/int>");
	ConsoleUtils::logToConsole("    generate tree");
}