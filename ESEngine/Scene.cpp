#include "Scene.h"

Scene::Scene() {
	lightsManager.reset(new LightsManager());
	physicsManager.reset(new PhysicsManager());
}

void Scene::setActiveCamera(GameObject *gameObject) {
	activeCamera = (Camera*)gameObject->getComponent(CAMERA);
}

GameObject* Scene::addGameObject(unique_ptr<GameObject> go) {
	Uuid uuid = go->id;
	gameObjects[uuid] = std::move(go);
	if (gameObjects[uuid]->getComponent(RIGIDBODY) != nullptr)
		physicsManager->addRigidBody((RigidBody*)gameObjects[uuid]->getComponent(RIGIDBODY));
	if (gameObjects[uuid]->getComponent(LIGHT) != nullptr)
		lightsManager->addLight((Light*)gameObjects[uuid]->getComponent(LIGHT));
	return gameObjects[uuid].get();
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
		lightsManager->removeLight((Light*) gameObjects[uuid]->getComponent(LIGHT));
	gameObjects.erase(uuid);
}

void Scene::update(double dt, InputState &inputState) {
	physicsManager->step(dt);

	for(const auto &node : gameObjects) {
		GameObject *go = node.second.get();
		
		RigidBody *rigidBody = (RigidBody*)go->getComponent(RIGIDBODY);
		if (rigidBody != nullptr) {
			rigidBody->updateTransform();
		}
		
		for (auto & behaviour : go->getComponents(BEHAVIOUR)) {
			((Behaviour*)behaviour)->update(dt, inputState);
		}
	}
}

void Scene::renderFrame(Renderer &renderer) {

	Shader::updateViewMatrix(activeCamera->getViewMatrix());
	lightsManager->updateLights(activeCamera->position);

	for(const auto &node : gameObjects) {
		GameObject *go = node.second.get();
		for (auto &	renderable : go->getComponents(RENDERABLE)) {
			((Renderable*)renderable)->draw(renderer);
		}
	}
}