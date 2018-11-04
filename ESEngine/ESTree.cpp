#include "Engine.h"
#include "Window.h"
#include "TreeScene.h"
#include "TestScene.h"
#include "PhysicsScene.h"
#include "SingleTreeScene.h"
#include "Logger.h"
#include "ConsoleInterpreter.h"
#include "ConsoleTreeCommand.h"
#include "ConsoleCubeCommand.h"
#include "ConsoleModelExportCommand.h"
#include <boost/program_options.hpp>

using namespace glm;
using namespace boost::program_options;

void testScene(SceneManager *sceneManager);
void treeScene(SceneManager *sceneManager);
void physicsScene(SceneManager *sceneManager);
void singleTreeScene(SceneManager *sceneManager);

Logger logger("ESTree");

Engine* initEngine(int width, int height);

int main(int argc, char* argv[]) {
	int width, height, scene = 0;

	try {
		options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("width,W", value<int>()->default_value(1024), "set window width")
			("height,H", value<int>()->default_value(768), "set window height")
			("scene,s", value<int>()->default_value(0), "load one of four scenes (0-3)");

		variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);
		notify(vm);

		if (vm.count("help")) {
			cout << desc << "\n";
			return 0;
		}

		width = vm["width"].as<int>();
		height = vm["height"].as<int>();
		scene = vm["scene"].as<int>();
	}
	catch (exception& e) {
		cerr << "error: " << e.what() << "\n";
		return 1;
	}
	catch (...) {
		cerr << "Exception of unknown type!\n";
		return 1;
	}
	
	logger.log(INFO, "Starting engine..");
	Engine* engine = initEngine(width, height);
	logger.log(INFO, "Creating scene..");
	SceneManager* sceneManager = engine->getSceneManager();

	switch (scene) {
	case 0:
		singleTreeScene(sceneManager);
		break;
	case 1:
		physicsScene(sceneManager);
		break;
	case 2:
		treeScene(sceneManager);
		break;
	case 3:
		testScene(sceneManager);
		break;
	}
	
	logger.log(INFO, "Scene created. Rendering..");

	ConsoleInterpreter::addCustomCommand(make_shared<ConsoleTreeCommand>(ConsoleTreeCommand()));
	ConsoleInterpreter::addCustomCommand(make_shared<ConsoleCubeCommand>(ConsoleCubeCommand()));
	ConsoleInterpreter::addCustomCommand(make_shared<ConsoleModelExportCommand>(ConsoleModelExportCommand()));

	engine->startRendering();

	delete(engine);

	return 0;
}

void physicsScene(SceneManager *sceneManager) {
	sceneManager->setActiveScene(shared_ptr<Scene>(new PhysicsScene()));
}

void treeScene(SceneManager *sceneManager) {
	sceneManager->setActiveScene(shared_ptr<Scene>(new TreeScene()));
}

void testScene(SceneManager *sceneManager) {
	sceneManager->setActiveScene(shared_ptr<Scene>(new TestScene()));
}

void singleTreeScene(SceneManager *sceneManager) {
	sceneManager->setActiveScene(shared_ptr<Scene>(new SingleTreeScene()));
}

Engine* initEngine(int width, int height) {
	Engine* engine = new Engine();
	Window* window = engine->initialize(width, height);
	return engine;
}