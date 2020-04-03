#ifndef ENGINE_H
#define ENGINE_H

#include "Screen.h"
#include "GraphicEngine/Core/Window.h"
#include "GraphicEngine/Core/Renderer.h"
#include "Engine/Manager/SceneManager.h"
#include "Engine/Manager/LightsManager.h"
#include "Logger.h"
#include "Engine/Manager/Context.h"

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
	double lastTime = 0.;

	void renderingLoop();
};

#endif