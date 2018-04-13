#include "Scene.h"

Scene::Scene() {
	lightsManager.reset(new LightsManager());
	physicsManager.reset(new PhysicsManager());
	mouseManager.reset(new MouseManager(physicsManager.get()));
}

void Scene::setActiveCamera(GameObject *gameObject) {
	activeCamera = (Camera*)gameObject->getComponent(CAMERA);
	mouseManager->setCamera((CameraBehaviour*)gameObject->getComponent(BEHAVIOUR), activeCamera);
}

void Scene::setSkybox(unique_ptr<Skybox> skybox) {
	this->skybox = std::move(skybox);
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

void Scene::setFrameBuffer(unique_ptr<FrameBuffer> frameBuffer) {
	this->sceneFrameBuffer = move(frameBuffer);
}

void Scene::update(double &dt, InputState &inputState) {
	physicsManager->step(dt);
	mouseManager->update(dt, inputState);

	for(const auto &node : gameObjects) {
		GameObject *go = node.second.get();
		if (go->getComponent(CAMERA) != NULL)
			continue;
		
		RigidBody *rigidBody = (RigidBody*)go->getComponent(RIGIDBODY);
		if (rigidBody != nullptr) {
			if (rigidBody->needsReload)
				physicsManager->reloadRigidBody(rigidBody);
			rigidBody->update();
		}
		
		for (auto & behaviour : go->getComponents(BEHAVIOUR)) {
			((Behaviour*)behaviour)->update(dt, inputState);
		}
	}
}

void Scene::renderObjects(Renderer &renderer) {
	for (const auto &node : gameObjects) {
		GameObject *go = node.second.get();
		for (auto & renderable : go->getComponents(RENDERABLE)) {
			((Renderable*)renderable)->draw(renderer);
		}
	}
}

void Scene::renderObjectsUsingShader(Renderer &renderer, Shader *shader) {
	for (const auto &node : gameObjects) {
		GameObject *go = node.second.get();
		for (auto & renderable : go->getComponents(RENDERABLE)) {
			((Renderable*)renderable)->draw(renderer, shader);
		}
	}
}

void Scene::renderSkybox(Renderer &renderer) {
	if (skybox != nullptr) {
		skybox->updatePosition(activeCamera->position);
		skybox->draw(renderer);
	}
}

function<void(Renderer&, Shader*)> Scene::prepareDrawObjectsCall() {
	function<void(Renderer&, Shader*)> renderObjectsFunction;
	renderObjectsFunction = [this](Renderer &renderer, Shader *shader) { renderObjectsUsingShader(renderer, shader); };
	return renderObjectsFunction;
}

void Scene::renderFrame(Renderer &renderer) {
	lightsManager->updateLights(activeCamera->position, renderer, prepareDrawObjectsCall());
	Shader::updateViewMatrix(activeCamera->getViewMatrix());

	preSceneRenderRoutine(renderer);

	renderObjects(renderer);
	renderSkybox(renderer);
	
	postSceneRenderRoutine(renderer);
	drawGui();
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

void Scene::drawGui() {
	DepthFrameBuffer::drawDepthFrame();
}