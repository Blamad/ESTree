#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <map>
#include <GLM/gtc/matrix_transform.hpp>

#include "GameObject.h"
#include "InputState.h"
#include "Renderer.h"
#include "LightsManager.h"
#include "PhysicsManager.h"

#include "Renderable.h"
#include "Behaviour.h"
#include "Camera.h"
#include "Skybox.h"

using namespace std;

class Scene {
public:
	Scene();

	void update(double dt, InputState &inputState);
	void renderFrame(Renderer &renderer);

	GameObject* addGameObject(unique_ptr<GameObject> go);
	GameObject* createGameObject();
	void removeGameObject(GameObject *gameObject);

	void setActiveCamera(GameObject *gameObject);
	void setSkybox(unique_ptr<Skybox> skybox);

private:
	Camera *activeCamera;
	unique_ptr<Skybox> skybox;
	unique_ptr<LightsManager> lightsManager;
	unique_ptr<PhysicsManager> physicsManager;
	std::map<Uuid, unique_ptr<GameObject>> gameObjects;
};

#endif