#include "ConsoleTreeCommand.h"

bool ConsoleTreeCommand::processCommandLine(vector<string> line)  {
	if (line[0] != "rtree" && line[0] != "tree") {
		return false;
	}

	LindenmayerTree *selected = nullptr;
	bool rldTree = false;

	vec3 pos(0, 0, 0);
	string name = "Tree";
	LindenmayerTreeParams lindenmayerParameters = LindenmayerTreeParams("LindenmayerRules/randomTree.json");
	Material barkMaterial = Material::diffuseTextureOnly("Textures/barkTexture3.jpg");
	Material leavesMaterial = Material::diffuseTextureOnly("Textures/leaves3.png");

	if (line.size() > 1) {
		map<string, string> params = ConsoleUtils::generateParamsMap(line);

		if (line[0] == "rtree") {
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
					if (params["file"].length() > 2) {
						lindenmayerParameters = LindenmayerTreeParams("LindenmayerRules/" + params["file"]);
					}
					else {
						int index = stoi(params["file"]);
						static vector<string> treeParams = {
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
							"parametricTernaryTreeA.json"
							//15
						};
						lindenmayerParameters = LindenmayerTreeParams("LindenmayerRules/" + (treeParams.size() > index ? treeParams[index] : treeParams[0]));
					}
					params.erase("file");
					continue;
				}
				if (params.find("bark") != params.end()) {
					if (params["bark"].length() > 2) {
						barkMaterial = Material::diffuseTextureOnly("Textures/" + params["bark"]);
					}
					else {
						int index = stoi(params["bark"]);
						static vector<string> barkTextures = {
							"barkTexture1.jpg",
							"barkTexture2.jpg",
							"barkTexture3.jpg",
							"barkTexture4.jpg",
							"barkTexture5.jpg",
							"treeTexture1.jpg",
							"treeTexture2.jpg"
						};
						barkMaterial = Material::diffuseTextureOnly("Textures/" + (barkTextures.size() > index ? barkTextures[index] : barkTextures[0]));
					}
					params.erase("bark");
					continue;
				}
				if (params.find("leaves") != params.end()) {
					if (params["leaves"].length() > 2) {
						leavesMaterial = Material::diffuseTextureOnly("Textures/" + params["leaves"]);
					}
					else {
						int index = stoi(params["leaves"]);
						static vector<string> leaves = {
							"leaves1.png",
							"leaves2.png",
							"leaves3.png",
							"leaves4.png",
							"leaves5.png",
							"arrow.png"
						};
						leavesMaterial = Material::diffuseTextureOnly("Textures/" + (leaves.size() > index ? leaves[index] : leaves[0]));
					}
					params.erase("leaves");
					continue;
				}

				ConsoleUtils::logToConsole("Unknown tree parameter!");
			}

			GameObject *go = LindenmayerTreeFactory::getInstance().generateTree(lindenmayerParameters, name, barkMaterial, leavesMaterial, pos);
			if (rldTree) {
				Context::getSceneManager()->getActiveScene()->removeGameObject(selected);
				Context::getMouseManager()->setSelectedGameObject(go);
			}
		}
		else {
			ConsoleUtils::logToConsole("Command parameters does not match expected values");
		}
	}
	return true;
}

void ConsoleTreeCommand::printHelpText() {
	ConsoleUtils::logToConsole(" - rtree pos <vec3> file <string/int> name <string> bark <string/int> leaves <string/int>");
	ConsoleUtils::logToConsole("    reloads selected tree");
	ConsoleUtils::logToConsole(" - tree pos <vec3> file <string/int> name <string> bark <string/int> leaves <string/int>");
	ConsoleUtils::logToConsole("    generate tree");
}