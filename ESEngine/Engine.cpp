#include "Engine.h"

#define OPENGL

#ifdef OPENGL
#include "GLWindow.h"
#include "GLRenderer.h"
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
		logger.log(INFO, "Rendering started");
		renderingLoop();
	}
}

Window* Engine::initialize(int width, int height) {
#ifdef OPENGL
	window.reset(new GLWindow(width, height));
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

	while (!window->shouldClose()) {
		double currentTime = window->getTime();
		double deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		window->poolInputEvents();
		sceneManager->getActiveScene()->update(deltaTime, *inputState);
		inputState->clearMouseEvents();

		window->prepareFrameRendering();
		sceneManager->getActiveScene()->renderFrame(*renderer);
		window->finishFrameRendering();
	}
}