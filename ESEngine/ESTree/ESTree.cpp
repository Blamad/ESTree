#include "Engine/Core/Engine.h"
#include "GraphicEngine/Core/Window.h"
#include "ESTree/Scenes/TreeScene.h"
#include "ESTree/Scenes/TestScene.h"
#include "ESTree/Scenes/PhysicsScene.h"
#include "ESTree/Scenes/SingleTreeScene.h"
#include "ESTree/Scenes/ForestScene.h"
#include "Engine/Core/Logger.h"
#include "Engine/UI/ConsoleInterpreter.h"
#include "ESTree/Console/ConsoleTreeCommand.h"
#include "ESTree/Console/ConsoleCubeCommand.h"
#include "ESTree/Console/ConsoleModelExportCommand.h"
#include <boost/program_options.hpp>

void testScene(SceneManager *sceneManager);
void treeScene(SceneManager *sceneManager);
void physicsScene(SceneManager *sceneManager);
void singleTreeScene(SceneManager *sceneManager);
void forestScene(SceneManager *sceneManager);

Logger logger("ESTree");

Engine* initEngine(int width, int height, bool fullScreenMode);

int main(int argc, char* argv[]) {
	int width, height, scene = 0;
	bool fullScreenMode = false;

	char buf[256];
	GetCurrentDirectoryA(256, buf);
	logger.log(LOG_INFO, std::string(buf) + '\\');

	try {
		boost::program_options::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("width,W", boost::program_options::value<int>()->default_value(1366), "set window width")
			("height,H", boost::program_options::value<int>()->default_value(768), "set window height")
			("scene,s", boost::program_options::value<int>()->default_value(0), "load one of four scenes (0-3)")
			("fullscreen,f", boost::program_options::bool_switch(&fullScreenMode), "launch application in fullscreen mode");

		boost::program_options::variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm);

		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 0;
		}

		width = vm["width"].as<int>();
		height = vm["height"].as<int>();
		scene = vm["scene"].as<int>();
	}
	catch (std::exception& e) {
		std::cerr << "error: " << e.what() << "\n";
		return 1;
	}
	catch (...) {
		std::cerr << "Exception of unknown type!\n";
		return 1;
	}
	
	logger.log(LOG_INFO, "Starting engine..");
	Engine* engine = initEngine(width, height, fullScreenMode);
	logger.log(LOG_INFO, "Creating scene..");
	SceneManager* sceneManager = engine->getSceneManager();

	switch (scene) {
	case 0:
		singleTreeScene(sceneManager);
		break;
	case 1:
		physicsScene(sceneManager);
		break;
	case 2:
		forestScene(sceneManager);
		break;
	case 3:
		testScene(sceneManager);
		break;
	case 4:
		treeScene(sceneManager);
		break;
	}
	
	logger.log(LOG_INFO, "Scene created. Rendering..");

	ConsoleInterpreter::addCustomCommand(std::make_shared<ConsoleTreeCommand>(ConsoleTreeCommand()));
	ConsoleInterpreter::addCustomCommand(std::make_shared<ConsoleCubeCommand>(ConsoleCubeCommand()));
	ConsoleInterpreter::addCustomCommand(std::make_shared<ConsoleModelExportCommand>(ConsoleModelExportCommand()));

	engine->startRendering();
	logger.log(LOG_INFO, "Quitting..");

	delete(engine);

	return 0;
}

void physicsScene(SceneManager *sceneManager) {
	sceneManager->setActiveScene(std::shared_ptr<Scene>(new PhysicsScene()));
}

void treeScene(SceneManager *sceneManager) {
	sceneManager->setActiveScene(std::shared_ptr<Scene>(new TreeScene()));
}

void forestScene(SceneManager *sceneManager) {
	sceneManager->setActiveScene(std::shared_ptr<Scene>(new ForestScene()));
}

void testScene(SceneManager *sceneManager) {
	sceneManager->setActiveScene(std::shared_ptr<Scene>(new TestScene()));
}

void singleTreeScene(SceneManager *sceneManager) {
	sceneManager->setActiveScene(std::shared_ptr<Scene>(new SingleTreeScene()));
}

Engine* initEngine(int width, int height, bool fullScreenMode) {
	Engine* engine = new Engine();
	Window* window = engine->initialize(width, height, fullScreenMode);
	return engine;
}