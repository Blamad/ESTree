#include "ConsoleInterpreter.h"

#include "UIManager.h"
#include "SceneManager.h"

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

		if (line[0] == "tree" && line.size() == 2) {
			vec3 position = parseCoords(line[1]);
			LindenmayerTreeParams params = LindenmayerTreeParams("LindenmayerRules/fibbonacciTree.json");

			unique_ptr<GameObject> go = unique_ptr<GameObject>(new LindenmayerTree(params, Material::bark3(), Material::leaves3(), false, false));
			go->name = "Tree";
			Transform *transform = (Transform*)go->getComponent(TRANSFORM);
			transform->translate(position);
			((LindenmayerTree*)go.get())->generate();

			shared_ptr<RigidBody> rigidBody = shared_ptr<RigidBody>(new RigidBody());
			go->addComponent(rigidBody);
			rigidBody->initAsAHullShape(0);

			Context::getSceneManager()->addGameObject(move(go));
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
	}
	catch (exception e) {

	}
}