#include "Scene.h"

boost::uuids::random_generator Scene::uuidGenerator;

Scene::Scene() {

}

void Scene::setActiveCamera(GameObject *gameObject) {
	activeCamera = (Camera*)gameObject->getComponent(CAMERA);
}

GameObject* Scene::createGameObject() {
	Uuid id = generateUuid();
	gameObjects[id] = unique_ptr<GameObject>(new GameObject(id));
	return gameObjects[id].get();
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

Uuid Scene::generateUuid() {
	return uuidGenerator();
}