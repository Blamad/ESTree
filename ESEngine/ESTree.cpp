#include "Engine.h"
#include "Window.h"

void createCamera(Scene* scene);
Engine* initEngine();

int main() {
	Engine* engine = initEngine();
	Scene* scene = engine->getSceneManager()->getActiveScene();
	createCamera(scene);

	engine->startRendering();

	return 0;
}

Engine* initEngine() {
	Engine* engine = new Engine();
	Window* window = engine->initialize(800, 600);
	return engine;
}

void createCamera(Scene *scene) {
	GameObject *go = scene->createGameObject();
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	go->addComponent(camera);
	scene->setActiveCamera(go);
}