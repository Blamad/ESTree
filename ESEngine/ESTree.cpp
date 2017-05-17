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
	createCamera(sceneManager, vec3(0.0f, 0.0f, 8.0f), -90, 0);
	Transform* transform;

	GameObject* cube = createColoredCube(sceneManager, vec4(1, 0, 0, 1));
	transform = getTransform(cube);
	transform->translate(vec3(2, .0, .0));
	
	GameObject* cube1 = createCube(sceneManager);
	transform = getTransform(cube1);
	transform->translate(vec3(-2, .0, .0));
	
	GameObject* lampCube = createLampCube(sceneManager, vec4(1, 1, 1, 1));
	transform = getTransform(lampCube);
	transform->translate(vec3(0, 2.0, .0));
	PointLight *pLight = (PointLight*)lampCube->getComponent(LIGHT);
	
	GameObject* lampCube2 = createLampCube(sceneManager, vec4(1, 1, 1, 1));
	transform = getTransform(lampCube2);
	transform->translate(vec3(0, -4.0, .0));
	
	GameObject* lampCube3 = createLampCube(sceneManager, vec4(1, 1, 1, 1));
	transform = getTransform(lampCube3);
	transform->translate(vec3(-2.0, -1.0, 4.0));
	
	GameObject* dirLightObject = sceneManager->addGameObject(unique_ptr<GameObject>(new GameObject()));
	dirLightObject->addComponent(sceneManager->createDirectionalLight());
	DirectionalLight *light = (DirectionalLight*) dirLightObject->getComponent(LIGHT);
	light->directory = vec3(-0.2f, -1.0f, -0.3f);

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
	GameObject* go = sceneManager->addGameObject(unique_ptr<GameObject>(new LampCube(color)));
	go->addComponent(sceneManager->createPointLight());
	Transform* transform = getTransform(go);
	transform->scale(vec3(.2, .2, .2));
	return go;
}

Engine* initEngine() {
	Engine* engine = new Engine();
	Window* window = engine->initialize(800, 600);
	return engine;
}