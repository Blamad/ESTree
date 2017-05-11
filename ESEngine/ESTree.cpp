#include "Engine.h"
#include "Window.h"
#include "Cube.h"
#include "LampCube.h"

using namespace glm;

GameObject* createCamera(SceneManager *sceneManager, vec3 position, float pitch, float yaw);
GameObject* createCube(SceneManager *sceneManager);
GameObject* createColoredCube(SceneManager *sceneManager, vec4 color);
GameObject* createLampCube(SceneManager *sceneManager, vec4 color);
Transform* getTransform(GameObject* gameObject);

Engine* initEngine();

int main() {
	Engine* engine = initEngine();
	SceneManager* sceneManager = engine->getSceneManager();
	createCamera(sceneManager, vec3(0.0f, 1.0f, -8.0f), 90, 0);
	Transform* transform;

	GameObject* cube = createColoredCube(sceneManager, vec4(1, 0, 0, 1));
	transform = getTransform(cube);
	transform->position = vec3(2, .0, .0);
	transform->rotation = vec4(.2f, .3f, .4f, 1);
	transform->scale = vec3(.5, .5, .5);

	GameObject* cube1 = createCube(sceneManager);
	transform = getTransform(cube1);
	transform->position = vec3(-2, .0, .0);
	transform->rotation = vec4(-.2f, -.3f, -.4f, 1);
	transform->scale = vec3(.5, .5, .5);

	GameObject* lampCube = createLampCube(sceneManager, vec4(1,1,1,1));
	transform = getTransform(lampCube);
	transform->position = vec3(0, 2.0, .0);
	transform->rotation = vec4(1, 1, 1, 0);
	transform->scale = vec3(.2, .2, .2);

	engine->startRendering();

	delete(engine);

	return 0;
}

Transform* getTransform(GameObject* gameObject) {
	return (Transform*)gameObject->getComponent(TRANSFORM);
}

GameObject* createCamera(SceneManager *sceneManager, vec3 position, float pitch, float yaw) {
	return sceneManager->createCamera(position, pitch, yaw);
}

GameObject* createCube(SceneManager *sceneManager) {
	return sceneManager->addGameObject(unique_ptr<GameObject>(new Cube(true)));
}

GameObject* createColoredCube(SceneManager *sceneManager, vec4 color) {
	return sceneManager->addGameObject(unique_ptr<GameObject>(new Cube(color)));
}

GameObject* createLampCube(SceneManager *sceneManager, vec4 color) {
	return sceneManager->addGameObject(unique_ptr<GameObject>(new LampCube(color)));
}

Engine* initEngine() {
	Engine* engine = new Engine();
	Window* window = engine->initialize(800, 600);
	return engine;
}