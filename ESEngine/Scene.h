#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <map>
#include <boost/uuid/uuid_generators.hpp>
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

	GameObject* createGameObject();
	void setActiveCamera(GameObject *gameObject);

private:
	Camera *activeCamera;

	std::map<Uuid, unique_ptr<GameObject>> gameObjects;
	static boost::uuids::random_generator uuidGenerator;

	Uuid generateUuid();
};

#endif