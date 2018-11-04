#include "ConsoleCubeCommand.h"

bool ConsoleCubeCommand::processCommandLine(vector<string> line) {
	if (line.at(0) == "cube") {

		map<string, string> params = ConsoleUtils::generateParamsMap(line);
		vec3 pos(0, 0, 0);
		string name = "CrateCube";

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
			ConsoleUtils::logToConsole("Unknown cube parameter!");
			break;
		}

		unique_ptr<GameObject> go = unique_ptr<GameObject>(new Cube(Material::container(), false, false));
		go->name = name;
		Transform *transform = (Transform*)go->getComponent(TRANSFORM);
		transform->translate(pos);
		shared_ptr<RigidBody> rigidBody = shared_ptr<RigidBody>(new RigidBody());
		go->addComponent(rigidBody);
		rigidBody->initAsBox(1);
		rigidBody->makeDynamic();
		Context::getMouseManager()->setSelectedGameObject(go.get());
		Context::getSceneManager()->getActiveScene()->addGameObject(move(go));
		return true;
	}

	if (line.at(0) == "light") {

		map<string, string> params = ConsoleUtils::generateParamsMap(line);
		vec3 pos(0, 0, 0);
		string name = "light";
		PointLightStrength strength = PointLightStrength::STRONG;

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
			if (params.find("str") != params.end()) {
				switch (ConsoleUtils::strToInt(params["str"])) {
				case 0:
					strength = PointLightStrength::WEAK;
					break;
				case 1:
					strength = PointLightStrength::MEDIUM;
					break;
				case 2:
					strength = PointLightStrength::STRONG;
					break;
				default:
					ConsoleUtils::logToConsole("Invalid strength value, medium used");
					strength = PointLightStrength::MEDIUM;
				}
				params.erase("str");
				continue;
			}

			ConsoleUtils::logToConsole("Unknown light parameter!");
			break;
		}

		unique_ptr<GameObject> go = unique_ptr<GameObject>(new LampCube(vec4(1,1,1,1)));
		go->name = name;
		go->addComponent(shared_ptr<PointLight>(new PointLight(strength)));
		Transform *transform = (Transform*)go->getComponent(TRANSFORM);
		transform->translate(pos);
		transform->scale(vec3(0.5, 0.5, 0.5));
		shared_ptr<RigidBody> rigidBody = shared_ptr<RigidBody>(new RigidBody());
		go->addComponent(rigidBody);
		rigidBody->initAsBox(1);
		rigidBody->makeDynamic();
		Context::getMouseManager()->setSelectedGameObject(go.get());
		Context::getSceneManager()->getActiveScene()->addGameObject(move(go));
		return true;
	}

	return false;
}

void ConsoleCubeCommand::printHelpText() {
	ConsoleUtils::logToConsole(" - cube pos <vec3> name <string>");
	ConsoleUtils::logToConsole("    generate cube");
	ConsoleUtils::logToConsole(" - light pos <vec3> name <string> str <0-2>");
	ConsoleUtils::logToConsole("    generate point light source");
}