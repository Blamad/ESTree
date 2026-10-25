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
#include "ESTree/CommandLine.h"
#include <iostream>

#ifdef _WIN32
// Ask hybrid-graphics drivers (NVIDIA Optimus / AMD PowerXpress) for the discrete GPU.
extern "C" {
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

void testScene(SceneManager *sceneManager);
void treeScene(SceneManager *sceneManager);
void physicsScene(SceneManager *sceneManager);
void singleTreeScene(SceneManager *sceneManager);
void forestScene(SceneManager *sceneManager);

Logger logger("ESTree");

Engine* initEngine(int width, int height, bool fullScreenMode);

int main(int argc, char* argv[]) {
	CommandLine::Result cli = CommandLine::parse(argc, argv);
	if (!cli.ok) {
		std::cerr << "error: " << cli.error << "\n\n" << CommandLine::helpText();
		return 1;
	}
	if (cli.options.help) {
		std::cout << CommandLine::helpText();
		return 0;
	}

	int width = cli.options.width;
	int height = cli.options.height;
	int scene = cli.options.scene;
	bool fullScreenMode = cli.options.fullscreen;

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