#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"

class SceneManager {
public:
	SceneManager();

	Scene* getActiveScene();

private:
	shared_ptr<Scene> currentScene;
};

#endif