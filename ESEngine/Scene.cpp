#include "Scene.h"


Scene::Scene() {

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

void Scene::update(double dt, InputState &inputState) {
	for(const auto &node : gameObjects) {
		GameObject *go = node.second.get();
		Behaviour *behaviour = (Behaviour*)go->getComponent(BEHAVIOUR);
		if (behaviour != NULL) {
			behaviour->update(dt, inputState);
		}
	}
}

void Scene::renderFrame(const Renderer &renderer, float aspectRatio) {
	
	glm::mat4 projection = activeCamera->getProjectionMatrix(aspectRatio);
	glm::mat4 view = activeCamera->getViewMatrix();

	for(const auto &node : gameObjects) {
		GameObject *go = node.second.get();
		Renderable *renderable = (Renderable*) go->getComponent(RENDERABLE);
		if (renderable != nullptr) {
			renderable->draw(renderer, view, projection);
		}
	}
}