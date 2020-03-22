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

	Window* initialize(int width, int height, bool fullScreenMode);
	SceneManager* getSceneManager();

	void startRendering();

private:
	static Logger logger;

	std::unique_ptr<Window> window;
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<SceneManager> sceneManager;

	InputState* inputState = nullptr;
	double lastTime;

	void renderingLoop();
};

#endif