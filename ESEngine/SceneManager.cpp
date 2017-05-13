#include "SceneManager.h"
#include "CameraBehaviour.h"

SceneManager::SceneManager() {
	currentScene.reset(new Scene());
}

GameObject* SceneManager::addGameObject(unique_ptr<GameObject> go) {
	return currentScene->addGameObject(std::move(go));
}

Scene* SceneManager::getActiveScene() {
	return currentScene.get();
}

GameObject* SceneManager::createEmptyGameObject() {
	return currentScene->createGameObject();
}

shared_ptr<PointLight> SceneManager::createPointLight() {
	shared_ptr<PointLight> light = shared_ptr<PointLight>(new PointLight());
	currentScene->addLight(light);
	return light;
}

shared_ptr<DirectionalLight> SceneManager::createDirectionalLight() {
	shared_ptr<DirectionalLight> light = shared_ptr<DirectionalLight>(new DirectionalLight());
	currentScene->addLight(light);
	return light;
}

GameObject* SceneManager::createCamera(glm::vec3 position, float yaw, float pitch) {
	GameObject *go = createEmptyGameObject();
	shared_ptr<Camera> camera = shared_ptr<Camera>(new Camera(position, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch));
	go->addComponent(camera);
	shared_ptr<CameraBehaviour> cameraBehaviour = shared_ptr<CameraBehaviour>(new CameraBehaviour(camera.get()));
	go->addComponent(cameraBehaviour);
	currentScene->setActiveCamera(go);

	return go;
}