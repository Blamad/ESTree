#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include "PointLight.h"

class SceneManager {
public:
	SceneManager();

	Scene* getActiveScene();
	GameObject* addGameObject(unique_ptr<GameObject> go);
	GameObject* createEmptyGameObject();
	GameObject* createCamera(glm::vec3 position, float yaw, float pitch);
	shared_ptr<PointLight> createPointLight();
	shared_ptr<DirectionalLight> createDirectionalLight();

private:
	shared_ptr<Scene> currentScene;
};

#endif