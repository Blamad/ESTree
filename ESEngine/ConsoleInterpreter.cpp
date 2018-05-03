#include "ConsoleInterpreter.h"

#include "UIManager.h"
#include "SceneManager.h"

Logger ConsoleInterpreter::logger("ConsoleInterpreter");

void ConsoleInterpreter::processTreeCommand(map<string, string> params) {
	vec3 pos(0, 0, 0);
	string name = "Tree";
	string filename = "LindenmayerRules/randomTree.json";
	string barkTexture = "Textures/barkTexture3.jpg";
	string leavesTexture = "Textures/leaves3.png";

	while (!params.empty()) {
		if (params.find("pos") != params.end()) {
			pos = parseCoords(params["pos"]);
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
		
		logToConsole("Unknown tree parameter!");
		return;
	}

	LindenmayerTreeParams lindenmayerParameters = LindenmayerTreeParams(filename);
	createTree(lindenmayerParameters, name, Material::diffuseTextureOnly(barkTexture), Material::diffuseTextureOnly(leavesTexture), pos);
}

void ConsoleInterpreter::processCubeCommand(map<string, string> params) {
	vec3 pos(0, 0, 0);
	string name = "CrateCube";
	
	while (!params.empty()) {
		if (params.find("pos") != params.end()) {
			pos = parseCoords(params["pos"]);
			params.erase("pos");
			continue;
		}
		if (params.find("name") != params.end()) {
			name = params["name"];
			params.erase("name");
			continue;
		}
		
		logToConsole("Unknown cube parameter!");
		return;
	}
	
	unique_ptr<GameObject> go = unique_ptr<GameObject>(new Cube(Material::container(), false, false));
	go->name = name;
	Transform *transform = (Transform*)go->getComponent(TRANSFORM);
	transform->translate(pos);
	shared_ptr<RigidBody> rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1);
	rigidBody->makeDynamic();
}

void ConsoleInterpreter::processInput(string &input) {
	try {
		vector<string> line = split(input, " ");

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

		if (line[0] == "tree") {
			if (line.size() == 2) {
				if (line[1] == "rld") {
					LindenmayerTree *selected = (LindenmayerTree*) Context::getMouseManager()->getSelectedGameObject();
					if (selected != nullptr) {
						vec3 position = ((Transform*)selected->getComponent(TRANSFORM))->getPosition();
						GameObject *go = createTree(selected->getTreeParams(), selected->name, selected->getBarkMaterial(), selected->getLeavesMaterial(), position);
						Context::getSceneManager()->getActiveScene()->removeGameObject(selected);
						Context::getMouseManager()->setSelectedGameObject(go);
					}
				}
				return;
			}
			if (line.size() % 2 == 1) {
				map<string, string> params = generateParamsMap(line);
				processTreeCommand(params);
			}
			else {
				logToConsole("Command parameters does not match expected values");
			}
			return;
		}

		if (line[0] == "cube") {
			map<string, string> params = generateParamsMap(line);
			processCubeCommand(params);
			
			return;
		}

		if (line[0] == "rm") {
			GameObject *selected = Context::getMouseManager()->getSelectedGameObject();
			if (selected != nullptr) {
				string name = selected->name;
				Context::getSceneManager()->getActiveScene()->removeGameObject(selected);
				logToConsole("'" + name + "' removed");
			}
			return;
		}

		if (line[0] == "mv" && line.size() == 2) {
			GameObject *selected = Context::getMouseManager()->getSelectedGameObject();
			if (selected != nullptr) {
				vec3 position = parseCoords(line[1]);
				RigidBody *rigidbody = (RigidBody*)selected->getComponent(RIGIDBODY);
				rigidbody->translate(position);
			}
			return;
		}

		logToConsole("Unknown command '"+line[0]+"'");
	}
	catch (exception e) {
		logToConsole("Something went wrong!");
		logger.log(ERROR, e.what());
	}
}

void ConsoleInterpreter::displayHelp() {
	logToConsole("Available commands:");
	logToConsole(" - depth <on/off>");
	logToConsole("    display depth buffer");
	logToConsole(" - cube pos <vec3> name <string>");
	logToConsole("    generate cube");
	logToConsole(" - rm");
	logToConsole("    removes selected object");
	logToConsole(" - mv <vec3>");
	logToConsole("    moves selected object by given vector");
	logToConsole(" - tree rld");
	logToConsole("    reloads selected tree");
	logToConsole(" - tree pos <vec3> file <string> name <string> bark <string> leaves <string>");
	logToConsole("    generate tree");
}

GameObject* ConsoleInterpreter::createTree(LindenmayerTreeParams &params, string &name, Material &bark, Material &leaves, vec3 &position) {
	unique_ptr<GameObject> go = unique_ptr<GameObject>(new LindenmayerTree(params, bark, leaves, false, false));
	go->name = name;
	Transform *transform = (Transform*)go->getComponent(TRANSFORM);
	transform->translate(position);
	((LindenmayerTree*)go.get())->generate();

	shared_ptr<RigidBody> rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsAHullShape(0);
	GameObject *objectPtr = go.get();

	Context::getSceneManager()->addGameObject(move(go));
	return objectPtr;
}