#include "Scene.h"

Scene::Scene() {
	lightsManager.reset(new LightsManager());
}

void Scene::setActiveCamera(GameObject *gameObject) {
	activeCamera = (Camera*)gameObject->getComponent(CAMERA);
}

GameObject* Scene::addGameObject(unique_ptr<GameObject> go) {
	Uuid uuid = go->id;
	gameObjects[uuid] = std::move(go);
	return gameObjects[uuid].get();
}

Light* Scene::addLight(shared_ptr<Light> light) {
	lightsManager->addLight(light);
	return light.get();
}

GameObject* Scene::createGameObject() {
	unique_ptr<GameObject> go = unique_ptr<GameObject>(new GameObject());
	Uuid uuid = go->id;
	gameObjects[uuid] = std::move(go);
	return gameObjects[uuid].get();
}

//TODO Na razie to lezy!
void Scene::removeGameObject(GameObject *gameObject) {
	Uuid uuid = gameObject->id;

	if (gameObjects[uuid]->getComponent(LIGHT) != nullptr)
		//lightsManager->removeLight(dynamic_pointer_cast<Light> (gameObjects[uuid]->getComponent(LIGHT)));
	gameObjects.erase(uuid);
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
	lightsManager->updateLights(activeCamera->position);

	for(const auto &node : gameObjects) {
		GameObject *go = node.second.get();
		Renderable *renderable = (Renderable*) go->getComponent(RENDERABLE);
		if (renderable != nullptr) {
			renderable->draw(renderer);
		}
	}
}