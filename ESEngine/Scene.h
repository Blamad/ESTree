#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <map>
#include <functional>
#include <GLM/gtc/matrix_transform.hpp>

#include "GameObject.h"
#include "InputState.h"
#include "FrameBuffer.h"
#include "Renderer.h"
#include "LightsManager.h"
#include "PhysicsManager.h"
#include "MouseManager.h"

#include "Renderable.h"
#include "Behaviour.h"
#include "Camera.h"
#include "Skybox.h"
#include "DepthFrameBuffer.h"

using namespace std;

class Scene {
public:
	Scene();

	void update(double &dt, InputState &inputState);
	void renderObjectsUsingShader(Renderer &renderer, Shader *shader);
	void renderObjects(Renderer &renderer);
	void renderFrame(Renderer &renderer);

	GameObject* addGameObject(unique_ptr<GameObject> go);
	GameObject* createGameObject();
	void removeGameObject(GameObject *gameObject);

	void setActiveCamera(GameObject *gameObject);
	void setSkybox(unique_ptr<Skybox> skybox);
	void setFrameBuffer(unique_ptr<FrameBuffer> frameBuffer);

private:
	Camera *activeCamera;
	unique_ptr<Skybox> skybox;
	unique_ptr<MouseManager> mouseManager;
	unique_ptr<LightsManager> lightsManager;
	unique_ptr<PhysicsManager> physicsManager;
	map<Uuid, unique_ptr<GameObject>> gameObjects;
	unique_ptr<FrameBuffer> sceneFrameBuffer;

	void renderSkybox(Renderer &renderer);
	function<void(Renderer&, Shader*)> prepareDrawObjectsCall();

	void preSceneRenderRoutine(Renderer &renderer);
	void postSceneRenderRoutine(Renderer &renderer);

	void drawGui();
};

#endif