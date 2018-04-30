#include "ConsoleInterpreter.h"

#include "UIManager.h"
#include "SceneManager.h"

void ConsoleInterpreter::processTreeCommand(map<string, string> params) {
	vec3 pos(0, 0, 0);
	string name = "Tree";
	string filename = "LindenmayerRules/fibbonacciTree.json";
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

void ConsoleInterpreter::processInput(string &input) {
	try {
		vector<string> line = split(input, " ");

		if (line[0] == "depth" && line.size() == 2) {
			if (line[1] == "off")
				Context::getUIManager()->toggleDepthBufferComponent(false);
			if (line[1] == "on")
				Context::getUIManager()->toggleDepthBufferComponent(true);
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
				map<string, string> params;
				for (int i = 1; i < line.size(); i += 2) {
					params[line[i]] = line[i + 1];
				}
				processTreeCommand(params);
			}
			else {
				//ERROR, UNKNOWN INPUT PATTERN
			}

			return;
		}

		if (line[0] == "cube" && line.size() == 2) {
			vec3 position = parseCoords(line[1]);
			unique_ptr<GameObject> go = unique_ptr<GameObject>(new Cube(Material::container(), false, false));
			go->name = "CrateCube";
			Transform *transform = (Transform*)go->getComponent(TRANSFORM);
			transform->translate(position);
			shared_ptr<RigidBody> rigidBody = shared_ptr<RigidBody>(new RigidBody());
			go->addComponent(rigidBody);
			rigidBody->initAsBox(1);
			rigidBody->makeDynamic();

			Context::getSceneManager()->addGameObject(move(go));
			return;
		}

		if (line[0] == "rm") {
			GameObject *selected = Context::getMouseManager()->getSelectedGameObject();
			if (selected != nullptr) {
				Context::getSceneManager()->getActiveScene()->removeGameObject(selected);
				logToConsole("'" + selected->name + "' removed");
			}
		}

		if (line[0] == "mv" && line.size() == 2) {
			GameObject *selected = Context::getMouseManager()->getSelectedGameObject();
			if (selected != nullptr) {
				vec3 position = parseCoords(line[1]);
				RigidBody *rigidbody = (RigidBody*)selected->getComponent(RIGIDBODY);
				rigidbody->translate(position);
			}
		}

		logToConsole("Unknown command '"+line[0]+"'");
	}
	catch (exception e) {

	}
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