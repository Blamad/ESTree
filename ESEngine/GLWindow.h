#ifndef GL_WINDOW_H
#define GL_WINDOW_H
//GLAD
#include <glad/glad.h>
//GLFW
#include <GLFW/glfw3.h>
//GLM
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Window.h"
#include "InputState.h"
#include "Logger.h"

class GLWindow : public Window {
public:
	GLWindow(int width, int height, bool fullScreenMode) : Window(width, height, fullScreenMode) { };
	~GLWindow();

	bool shouldClose();
	bool initialize();
	void poolInputEvents();
	void prepareFrameRendering();
	void finishFrameRendering();
	void setViewport(int width, int height);

	void setCursorVisible();
	void setCursorHidden();
	void setCursorDisabled();

	double getTime();
	InputState* registerInputManager();

protected:

	//input managment stuff
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void mousePositionCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouseScrollCallback(GLFWwindow* window, double xpos, double ypos);
	static void mouseClickCallback(GLFWwindow* window, int button, int action, int mods);
	static void characterCallback(GLFWwindow* window, unsigned int codepoint, int mods);

private:
	static Logger logger;

	GLfloat lastTime;
	GLFWwindow* glfwWindow;
	std::unique_ptr<InputState> inputState;

	bool initGlfw();
	bool initGlad();
	void setupWindowParams();

	//input managment stuff
	static GLWindow* instance;
};
#endif