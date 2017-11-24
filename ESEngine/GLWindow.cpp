#include "GLWindow.h"
#include <iostream>

GLWindow* GLWindow::instance;

Logger GLWindow::logger("GLWindow");

GLWindow::~GLWindow() {
	glfwTerminate();
}

bool GLWindow::initialize() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//Inicjalizacja okna GLFW
	glfwWindow = glfwCreateWindow(width, height, "EsTree", nullptr, nullptr);
	if (glfwWindow == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(glfwWindow);
	setCursorVisible();

	//Inicjalizacja GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	//Rozmiar okna
	glViewport(0, 0, width, height);

	//Ustawienie opcji openGLa
	glEnable(GL_DEPTH_TEST);

	//Deprecated
	bool linesOnly = false;
	if(linesOnly)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glEnable(GL_CULL_FACE);
	
	//Ustawienie koloru czyszczenia ekranu
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

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

	return inputState.get();
}

bool GLWindow::shouldClose() {
	return glfwWindowShouldClose(glfwWindow);
}

void GLWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (instance) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);

		if (action == GLFW_PRESS)
			instance->inputState->setKeyPressed(key);
		else if (action == GLFW_RELEASE)
			instance->inputState->setKeyReleased(key);
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