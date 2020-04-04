#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <map>
#include <functional>
#include <GLM/gtc/matrix_transform.hpp>

#include "Engine/GameObject/GameObject.h"
#include "GraphicEngine/Input/InputState.h"
#include "GraphicEngine/Framebuffer/FrameBuffer.h"
#include "Engine/UI/UIComponent.h"
#include "GraphicEngine/Core/Renderer.h"
#include "Engine/Manager/LightsManager.h"
#include "Engine/Manager/PhysicsManager.h"
#include "Engine/Manager/MouseManager.h"

#include "Engine/GameObject/Renderable.h"
#include "Engine/GameObject/Behaviour.h"
#include "Engine/GameObject/Camera.h"
#include "Engine/GameObject/Skybox.h"
#include "GraphicEngine/Framebuffer/DepthFrameBuffer.h"
#include "Engine/Manager/UIManager.h"

class Scene {
public:
	Scene();
	~Scene();

	void update(double &dt, InputState &inputState);
	void renderObjectsUsingShader(Renderer &renderer, Shader *shader);
	void renderObjects(Renderer &renderer);
	void renderFrame(Renderer &renderer);

	GameObject* addGameObject(std::unique_ptr<GameObject> go);
	GameObject* createGameObject();
	void removeGameObject(GameObject *gameObject);

	void setActiveCamera(GameObject *gameObject);
	Camera * getActiveCamera();
	void setSkybox(std::unique_ptr<Skybox> skybox);
	void setFrameBuffer(std::unique_ptr<FrameBuffer> frameBuffer);

private:
	Camera *activeCamera = nullptr;
	std::unique_ptr<Skybox> skybox;
	std::unique_ptr<MouseManager> mouseManager;
	std::unique_ptr<LightsManager> lightsManager;
	std::unique_ptr<PhysicsManager> physicsManager;
	std::unique_ptr<UIManager> uiManager;
	std::map<Uuid, std::unique_ptr<GameObject>> gameObjects;
	std::unique_ptr<FrameBuffer> sceneFrameBuffer;

	void renderSkybox(Renderer &renderer);

	std::function<void(Renderer&, Shader*)> prepareDrawObjectsCall();

	void preSceneRenderRoutine(Renderer &renderer);
	void postSceneRenderRoutine(Renderer &renderer);
	void renderUI();
};

#endif