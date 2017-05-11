#include "Scene.h"

Scene::Scene() {
	initialized = false;
}

void Scene::setActiveCamera(GameObject *gameObject) {
	activeCamera = (Camera*)gameObject->getComponent(CAMERA);
}

GameObject* Scene::addGameObject(unique_ptr<GameObject> go) {
	Uuid uuid = go->id;
	gameObjects[uuid] = std::move(go);
	return gameObjects[uuid].get();
}

GameObject* Scene::createGameObject() {
	unique_ptr<GameObject> go = unique_ptr<GameObject>(new GameObject());
	Uuid uuid = go->id;
	gameObjects[uuid] = std::move(go);
	return gameObjects[uuid].get();
}

void initializeScene(Renderer &renderer, float aspectRatio) {

}

void Scene::update(double dt, InputState &inputState) {
	for(const auto &node : gameObjects) {
		GameObject *go = node.second.get();
		Behaviour *behaviour = (Behaviour*)go->getComponent(BEHAVIOUR);
		if (behaviour != nullptr) {
			behaviour->update(dt, inputState);
		}
	}
}

void Scene::renderFrame(Renderer &renderer) {

	Shader::updateViewMatrix(activeCamera->getViewMatrix());

	for(const auto &node : gameObjects) {
		GameObject *go = node.second.get();
		Renderable *renderable = (Renderable*) go->getComponent(RENDERABLE);
		if (renderable != nullptr) {
			renderable->draw(renderer);
		}
	}
}