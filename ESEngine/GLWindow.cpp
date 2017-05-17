#include "GLWindow.h"
#include <iostream>

GLWindow* GLWindow::instance;

GLWindow::~GLWindow()
{
	glfwTerminate();
}

bool GLWindow::initialize()
{
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
	glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Inicjalizacja GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	//Rozmiar okna
	glViewport(0, 0, width, height);

	//Ustawienie opcji openGLa
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	return true;
}

void GLWindow::poolInputEvents() {
	glfwPollEvents();
}

double GLWindow::getTime() {
	return glfwGetTime();
}

void GLWindow::prepareFrameRendering() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLWindow::finishFrameRendering() {
	glfwSwapBuffers(glfwWindow);
}

InputState* GLWindow::registerInputManager() {
	instance = this;
	inputState.reset(new InputState());

	glfwSetKeyCallback(glfwWindow, GLWindow::keyCallback);
	glfwSetCursorPosCallback(glfwWindow, GLWindow::mousePositionCallback);
	glfwSetScrollCallback(glfwWindow, GLWindow::mouseScrollCallback);

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