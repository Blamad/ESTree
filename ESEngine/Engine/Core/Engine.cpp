#include "Engine.h"

#define OPENGL

#ifdef OPENGL
#include "GraphicEngine/Core/GLWindow.h"
#include "GraphicEngine/Core/GLRenderer.h"
#endif

Logger Engine::logger("Engine");

Engine::Engine() {

}

Engine::~Engine() {
	window.reset();
	renderer.reset();
	sceneManager.reset();
}

void Engine::startRendering() {
	if (window) {
		logger.log(LOG_INFO, "Rendering started");
		renderingLoop();
	}
}

Window* Engine::initialize(int width, int height, bool fullScreenMode) {
#ifdef OPENGL
	window.reset(new GLWindow(width, height, fullScreenMode));
	renderer.reset(new GLRenderer());
#endif
	window->initialize();
	inputState = window->registerInputManager();
	Screen::setInstance(window.get());
	sceneManager.reset(new SceneManager());

	Shader::initializeMatricesUBO();
	LightsManager::initializeLightsUBO();
	Shader::updateProjectionMatrix(Camera::getProjectionMatrix());

	return window.get();
}

SceneManager* Engine::getSceneManager() {
	return sceneManager.get();
}

void Engine::renderingLoop() {
	Context::setStartTime();
	while (!window->shouldClose()) {
		double currentTime = window->getTime();
		Context::setTime(currentTime);
		double deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		window->poolInputEvents();
		sceneManager->getActiveScene()->update(deltaTime, *inputState);
		inputState->clearEvents();

		window->prepareFrameRendering();
		sceneManager->getActiveScene()->renderFrame(*renderer);
		window->finishFrameRendering();

		if (Context::getFrameCounterValue() > 1000) {
			Context::clearFrameCounter();
			Context::setStartTime();
		}
		Context::increaseFrameCounter();
	}
}