#ifndef ENGINE_H
#define ENGINE_H

#include "Window.h"
#include "Renderer.h"
#include "SceneManager.h"


class Engine {
public:
	Engine();
	~Engine();

	Window* initialize(int width, int height);
	SceneManager* getSceneManager();

	void startRendering();

private:
	unique_ptr<Window> window;
	unique_ptr<Renderer> renderer;
	unique_ptr<SceneManager> sceneManager;

	InputState* inputState;
	double lastTime;

	void renderingLoop();
};

#endif