#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <map>
#include <GLM/gtc/matrix_transform.hpp>

#include "GameObject.h"
#include "InputState.h"
#include "Renderer.h"

#include "Renderable.h"
#include "Behaviour.h"
#include "Camera.h"

using namespace std;

class Scene {
public:
	Scene();

	void update(double dt, InputState &inputState);
	void renderFrame(const Renderer &renderer, float aspectRatio);

	GameObject* addGameObject(unique_ptr<GameObject> go);
	GameObject* createGameObject();
	void setActiveCamera(GameObject *gameObject);

private:
	Camera *activeCamera;
	std::map<Uuid, unique_ptr<GameObject>> gameObjects;
};

#endif