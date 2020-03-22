#include "GLWindow.h"
#include <iostream>

GLWindow* GLWindow::instance = nullptr;

Logger GLWindow::logger("GLWindow");

GLWindow::~GLWindow() {
	instance = nullptr;
	inputState.release();
	glfwTerminate();
}

bool GLWindow::initialize() {
	if (!initGlfw() || !initGlad())
		return false;
	setupWindowParams();

	return true;
}

void GLWindow::setupWindowParams() {
	glViewport(0, 0, width, height);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	//glEnable(GL_CULL_FACE);

	setCursorVisible();
}

bool GLWindow::initGlad() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}

bool GLWindow::initGlfw() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	//Inicjalizacja okna GLFW
	glfwWindow = glfwCreateWindow(width, height, "EsTree", isFullScreenMode() ? glfwGetPrimaryMonitor() : nullptr, nullptr);
	if (glfwWindow == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(glfwWindow);
	return true;
}


void GLWindow::setViewport(int width, int height) {
	glViewport(0, 0, width, height);
}

void GLWindow::poolInputEvents() {
	glfwPollEvents();
}

double GLWindow::getTime() {
	return glfwGetTime();
}

void GLWindow::prepareFrameRendering() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLWindow::finishFrameRendering() {
	glfwSwapBuffers(glfwWindow);
}

void GLWindow::setCursorVisible() {
	glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void GLWindow::setCursorHidden() {
	glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void GLWindow::setCursorDisabled() {
	glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

InputState* GLWindow::registerInputManager() {
	instance = this;
	inputState.reset(new InputState());

	glfwSetKeyCallback(glfwWindow, GLWindow::keyCallback);
	glfwSetCursorPosCallback(glfwWindow, GLWindow::mousePositionCallback);
	glfwSetScrollCallback(glfwWindow, GLWindow::mouseScrollCallback);
	glfwSetMouseButtonCallback(glfwWindow, GLWindow::mouseClickCallback);
	glfwSetCharModsCallback(glfwWindow, GLWindow::characterCallback);

	return inputState.get();
}

bool GLWindow::shouldClose() {
	return glfwWindowShouldClose(glfwWindow);
}

void GLWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (instance) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
			instance = nullptr;
			return;
		}

		KeyState state;
		if (action == GLFW_REPEAT)
			state = REPEAT;
		if (action == GLFW_PRESS)
			state = PRESSED;
		if (action == GLFW_RELEASE)
			state = RELEASED;
		if(instance->inputState) 
		instance->inputState->pushKeyEvent(key, state);
	}
}

void GLWindow::mousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
	if (instance) {
		instance->inputState->pushMousePosition(xpos, ypos);
	}
}

void GLWindow::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	if (instance) {
		instance->inputState->pushMouseScroll(xoffset, yoffset);
	}
}

void GLWindow::characterCallback(GLFWwindow* window, unsigned int codepoint, int mods) {
	if (instance) {
		instance->inputState->pushCharEvent(codepoint);
	}
}

void GLWindow::mouseClickCallback(GLFWwindow* window, int button, int action, int mods) {
	if (instance) {
		MouseButton clickedButton = MOUSE_BUTTON_MIDDLE;
		bool pressed = false;
		
		if (button == GLFW_MOUSE_BUTTON_LEFT)
			clickedButton = MOUSE_BUTTON_LEFT;
		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
			clickedButton = MOUSE_BUTTON_RIGHT;
		
		if (action == GLFW_PRESS)
			pressed = true;

		ClickEvent click(clickedButton, pressed, instance->inputState->getLastMousePosition());

		if (mods == GLFW_MOD_SHIFT)
			click.modifier = MOUSE_MOD_SHIFT;
		else if (mods == GLFW_MOD_ALT)
			click.modifier = MOUSE_MOD_ALT;
		else if (mods == GLFW_MOD_CONTROL)
			click.modifier = MOUSE_MOD_CONTROL;

		instance->inputState->pushMouseClick(click);
	}
}