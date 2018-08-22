#ifndef ENGINE_H
#define ENGINE_H

#include "Screen.h"
#include "Window.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "LightsManager.h"
#include "Logger.h"
#include "Context.h"

class Engine {
public:
	Engine();
	~Engine();

	Window* initialize(int width, int height);
	SceneManager* getSceneManager();

	void startRendering();

private:
	static Logger logger;

	unique_ptr<Window> window;
	unique_ptr<Renderer> renderer;
	unique_ptr<SceneManager> sceneManager;

	InputState* inputState;
	double lastTime;

	void renderingLoop();
};

#endif