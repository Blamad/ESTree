#include "CameraBehaviour.h"
#include <GLFW/glfw3.h>

void CameraBehaviour::update(double dt, InputState &inputState) {
	processMouseMove(inputState);
	processMouseScroll(inputState);
	processKeypad(dt, inputState);
};

void CameraBehaviour::processMouseMove(InputState &inputState) {
	while (!inputState.mousePositionQueueEmpty()) {
		Point2d mousePos = inputState.popMousePosition();
		if (lastXPos != NULL && lastYPos != NULL)
			camera->processMouseMovement(mousePos.x - lastXPos, mousePos.y - lastYPos);
		lastXPos = mousePos.x;
		lastYPos = mousePos.y;
	}
};

void CameraBehaviour::processMouseScroll(InputState &inputState) {
	while (!inputState.mouseScrollQueueEmpty()) {
		Point2d scrollVal = inputState.popMouseScroll();
		camera->processMouseScroll(scrollVal.y);
	}
};

void CameraBehaviour::processKeypad(double dt, InputState &inputState) {
	if (inputState.isKeyPressed(GLFW_KEY_W))
		camera->processKeyboard(FORWARD, dt);
	if (inputState.isKeyPressed(GLFW_KEY_S))
		camera->processKeyboard(BACKWARD, dt);
	if (inputState.isKeyPressed(GLFW_KEY_A))
		camera->processKeyboard(LEFT, dt);
	if (inputState.isKeyPressed(GLFW_KEY_D))
		camera->processKeyboard(RIGHT, dt);
};