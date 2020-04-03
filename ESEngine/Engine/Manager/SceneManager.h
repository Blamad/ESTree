#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Engine/Scene/Scene.h"
#include "Engine/GameObject/PointLight.h"
#include "Context.h"

class SceneManager {
public:
	SceneManager();
	~SceneManager();

	Scene* getActiveScene();
	Scene* setActiveScene(std::shared_ptr<Scene> scene);
	GameObject* addGameObject(std::unique_ptr<GameObject> go);
	GameObject* createEmptyGameObject();
	GameObject* createCamera(glm::vec3 position, float yaw, float pitch);

private:
	std::shared_ptr<Scene> currentScene;
};

#endif