#include "ConsoleTreeCommand.h"
#include <sys/stat.h>

#include <cstdint>
#include <exception>
#include <limits>
#include <stdexcept>

#include <treegen/Error.h>
#include <treegen/TreeParams.h>

namespace {

// The in-game console shows one line per message: no line breaks, and not wider than it can usefully display.
std::string oneLine(std::string message) {
	for (char &c : message)
		if (c == '\n' || c == '\r' || c == '\t')
			c = ' ';
	const size_t limit = 200;
	if (message.size() > limit)
		message = message.substr(0, limit - 3) + "...";
	return message;
}

// Accepts digits only; 0 is allowed and means "no seed given" (a random one is drawn).
bool parseSeed(const std::string &text, uint32_t &seed) {
	if (text.empty() || text.size() > 10 || text.find_first_not_of("0123456789") != std::string::npos)
		return false;
	const unsigned long long value = std::stoull(text);
	if (value > (std::numeric_limits<uint32_t>::max)())
		return false;
	seed = static_cast<uint32_t>(value);
	return true;
}

}

bool ConsoleTreeCommand::processCommandLine(std::vector<std::string> line)  {
	if (line.at(0) != "rtree" && line.at(0) != "tree") {
		return false;
	}

	// A bad rule file or a failing generation is reported in the console; the command counts as handled.
	try {
		return generateFromCommandLine(line);
	}
	catch (const treegen::Error &error) {
		ConsoleUtils::logToConsole("Tree error: " + oneLine(error.what()));
	}
	catch (const std::exception &error) {
		ConsoleUtils::logToConsole("Tree command failed: " + oneLine(error.what()));
	}
	return true;
}

bool ConsoleTreeCommand::generateFromCommandLine(std::vector<std::string> &line) {
	LindenmayerTree *selected = nullptr;
	bool rldTree = false;

	glm::vec3 pos(0, 0, 0);
	std::string name = "Tree";
	uint32_t requestedSeed = 0;
	treegen::TreeParams lindenmayerParameters = treegen::loadTreeParams("Resources/LindenmayerRules/randomTree.json");
	Material barkMaterial = Material::diffuseTextureOnly("Resources/Textures/barkTexture3.jpg");
	Material leavesMaterial = Material::diffuseTextureOnly("Resources/Textures/leaves3.png");

	std::map<std::string, std::string> params = ConsoleUtils::generateParamsMap(line);

	if (line.at(0) == "rtree") {
		selected = dynamic_cast<LindenmayerTree*>(Context::getMouseManager()->getSelectedGameObject());
		if (selected != nullptr) {
			rldTree = true;
			pos = ((Transform*)selected->getComponent(TRANSFORM))->getPosition();
			name = selected->name;
			lindenmayerParameters = selected->getTreeParams();
			// rtree re-rolls: a seed written into the rule file would give the very same tree again
			lindenmayerParameters.seed = 0;
			barkMaterial = selected->getBarkMaterial();
			leavesMaterial = selected->getLeavesMaterial();
		}
		else {
			ConsoleUtils::logToConsole(Context::getMouseManager()->getSelectedGameObject() == nullptr ? "No tree selected!" : "Selected object is not a tree!");
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
				lindenmayerParameters = treegen::loadTreeParams(filePath);
				params.erase("file");
				continue;
			}
			if (params.find("seed") != params.end()) {
				if (!parseSeed(params["seed"], requestedSeed)) {
					ConsoleUtils::logToConsole("Seed must be a whole number from 0 to 4294967295: " + params["seed"]);
					return true;
				}
				params.erase("seed");
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

			// Without an erase this loop never ended on a misspelt parameter.
			ConsoleUtils::logToConsole("Unknown tree parameter: " + params.begin()->first);
			return true;
		}
	}
	else {
		ConsoleUtils::logToConsole("Command parameters does not match expected values");
	}

	if (lindenmayerParameters.seed != 0 && requestedSeed != 0 && lindenmayerParameters.seed != requestedSeed) {
		ConsoleUtils::logToConsole("The rule file fixes the seed (" + std::to_string(lindenmayerParameters.seed) + "), 'seed " + std::to_string(requestedSeed) + "' is ignored");
	}

	// On an error nothing below runs: for rtree the old tree stays where it is.
	LindenmayerTree *go = LindenmayerTreeFactory::getInstance().generateTree(lindenmayerParameters, name, barkMaterial, leavesMaterial, pos, requestedSeed);
	if (rldTree) {
		Context::getSceneManager()->getActiveScene()->removeGameObject(selected);
	}
	Context::getMouseManager()->setSelectedGameObject(go);
	ConsoleUtils::logToConsole("Tree '" + name + "' generated, seed " + std::to_string(go->getSeed()));

	return true;
}

void ConsoleTreeCommand::printHelpText() {
	ConsoleUtils::logToConsole(" - rtree pos <vec3> file <string/int> name <string> bark <string/int> leaves <string/int> seed <int>");
	ConsoleUtils::logToConsole("    reloads selected tree with its parameters and a NEW random seed;");
	ConsoleUtils::logToConsole("    'seed <int>' reuses a seed (the log line of a tree shows its seed)");
	ConsoleUtils::logToConsole(" - tree pos <vec3> file <string/int> name <string> bark <string/int> leaves <string/int> seed <int>");
	ConsoleUtils::logToConsole("    generate tree; same seed + same file = same tree.");
	ConsoleUtils::logToConsole("    A seed written in the rule file wins over 'seed <int>'");
}