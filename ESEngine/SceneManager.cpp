#include "SceneManager.h"

SceneManager::SceneManager() {
	currentScene.reset(new Scene());
}

Scene* SceneManager::getActiveScene() {
	return currentScene.get();
}