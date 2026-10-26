#include "Scene.h"

Logger Scene::logger("Scene");

Scene::Scene() {
	lightsManager.reset(new LightsManager());
	physicsManager.reset(new PhysicsManager());
	mouseManager.reset(new MouseManager(physicsManager.get()));
	uiManager.reset(new UIManager());
}

Scene::~Scene() {
	// Managers keep raw pointers into the objects' components, and the Bullet world dereferences every
	// body still registered when it is destroyed. So: unregister, destroy the objects while the managers
	// are still alive, and let the remaining members die in reverse declaration order.
	for (auto &node : gameObjects) {
		unregisterFromManagers(*node.second);
	}
	gameObjects.clear();
}

void Scene::setActiveCamera(GameObject *gameObject) {
	activeCamera = gameObject->getComponentAs<Camera>(CAMERA);
	mouseManager->setCamera(gameObject->getComponentAs<CameraBehaviour>(BEHAVIOUR), activeCamera);
}

Camera* Scene::getActiveCamera() {
	return activeCamera;
}

void Scene::setSkybox(std::unique_ptr<Skybox> skybox) {
	this->skybox = std::move(skybox);
}

GameObject* Scene::addGameObject(std::unique_ptr<GameObject> go) {
	GameObject *added = go.get();
	gameObjects[added->id] = std::move(go);

	if (RigidBody *rigidBody = added->getComponentAs<RigidBody>(RIGIDBODY))
		physicsManager->addRigidBody(rigidBody);
	if (Light *light = added->getComponentAs<Light>(LIGHT))
		lightsManager->addLight(light);
	return added;
}

GameObject* Scene::createGameObject() {
	std::unique_ptr<GameObject> go = std::unique_ptr<GameObject>(new GameObject());
	GameObject *created = go.get();
	gameObjects[created->id] = std::move(go);
	return created;
}

void Scene::removeGameObject(GameObject *gameObject) {
	if (gameObject == nullptr)
		return;

	auto node = gameObjects.find(gameObject->id);
	if (node == gameObjects.end()) {
		logger.log(LOG_WARN, "removeGameObject: object does not belong to this scene");
		return;
	}

	// Drop every raw pointer to the object before it dies.
	mouseManager->forgetGameObject(gameObject);
	unregisterFromManagers(*node->second);
	gameObjects.erase(node);
}

void Scene::unregisterFromManagers(GameObject &gameObject) {
	if (Light *light = gameObject.getComponentAs<Light>(LIGHT))
		lightsManager->removeLight(light);
	if (RigidBody *rigidBody = gameObject.getComponentAs<RigidBody>(RIGIDBODY))
		physicsManager->removeRigidBody(rigidBody);
}

void Scene::setFrameBuffer(std::unique_ptr<FrameBuffer> frameBuffer) {
	this->sceneFrameBuffer = move(frameBuffer);
}

void Scene::update(double &dt, InputState &inputState) {
	physicsManager->update(dt);
	uiManager->update(dt, inputState);
	mouseManager->update(dt, inputState);

	for(const auto &node : gameObjects) {
		GameObject *go = node.second.get();
		if (go->getComponent(CAMERA) != nullptr)
			continue;

		RigidBody *rigidBody = go->getComponentAs<RigidBody>(RIGIDBODY);
		if (rigidBody != nullptr) {
			if (rigidBody->needsReload) {
				physicsManager->reloadRigidBody(rigidBody);
				rigidBody->needsReload = false;
			}
			rigidBody->update();
		}

		for (Behaviour *behaviour : go->getComponentsAs<Behaviour>(BEHAVIOUR)) {
			behaviour->update(dt, inputState);
		}
	}
}

void Scene::renderObjects(Renderer &renderer) {
	for (const auto &node : gameObjects) {
		GameObject *go = node.second.get();
		for (Renderable *renderable : go->getComponentsAs<Renderable>(RENDERABLE)) {
			renderable->draw(renderer);
		}
	}
}

void Scene::renderObjectsUsingShader(Renderer &renderer, Shader *shader) {
	for (const auto &node : gameObjects) {
		GameObject *go = node.second.get();
		for (Renderable *renderable : go->getComponentsAs<Renderable>(RENDERABLE)) {
			renderable->draw(renderer, shader);
		}
	}
}

void Scene::renderSkybox(Renderer &renderer) {
	if (skybox != nullptr) {
		skybox->updatePosition(activeCamera->position);
		skybox->draw(renderer);
	}
}

std::function<void(Renderer&, Shader*)> Scene::prepareDrawObjectsCall() {
	std::function<void(Renderer&, Shader*)> renderObjectsFunction;
	renderObjectsFunction = [this](Renderer &renderer, Shader *shader) { renderObjectsUsingShader(renderer, shader); };
	return renderObjectsFunction;
}

void Scene::renderFrame(Renderer &renderer) {
	lightsManager->prepareShadowBuffer(activeCamera->position, renderer, prepareDrawObjectsCall());
	Shader::updateViewMatrix(activeCamera->getViewMatrix());

	preSceneRenderRoutine(renderer);

	renderObjects(renderer);
	renderSkybox(renderer);

	postSceneRenderRoutine(renderer);

	renderUI();
}

void Scene::preSceneRenderRoutine(Renderer &renderer) {
	if (sceneFrameBuffer != nullptr) {
		sceneFrameBuffer->mountFrameBuffer();
	}
}

void Scene::postSceneRenderRoutine(Renderer &renderer) {
	if (sceneFrameBuffer != nullptr) {
		sceneFrameBuffer->unmountFrameBuffer();
		sceneFrameBuffer->executeFrameBuffer(renderer);
	}
}

void Scene::renderUI() {
	uiManager->draw();
}