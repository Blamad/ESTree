#include "Engine.h"
#include "Window.h"
#include "TreeScene.h"
#include "TestScene.h"
#include "Logger.h"

using namespace glm;

GameObject* createCamera(SceneManager *sceneManager, vec3 position, float pitch, float yaw);

void testScene(SceneManager *sceneManager);
void treeScene(SceneManager *sceneManager);

Logger logger("ESTree");

Engine* initEngine();

int main() {
	logger.log(INFO, "Starting engine..");
	Engine* engine = initEngine();
	logger.log(INFO, "Creating scene..");
	SceneManager* sceneManager = engine->getSceneManager();

	//treeScene(sceneManager);
	testScene(sceneManager);
	logger.log(INFO, "Scene created. Rendering..");

	engine->startRendering();

	delete(engine);

	return 0;
}

void treeScene(SceneManager *sceneManager) {
	sceneManager->setActiveScene(shared_ptr<Scene>(new TreeScene()));
	//createCamera(sceneManager, vec3(0, 15, 35), -90, -10);
}

void testScene(SceneManager *sceneManager) {
	sceneManager->setActiveScene(shared_ptr<Scene>(new TestScene()));
	createCamera(sceneManager, vec3(0, 5, -12), 90, -10);
}

GameObject* createCamera(SceneManager *sceneManager, vec3 position, float pitch, float yaw) {
	return sceneManager->createCamera(position, pitch, yaw);
}

Engine* initEngine() {
	Engine* engine = new Engine();
	Window* window = engine->initialize(1024, 768);
	return engine;
}