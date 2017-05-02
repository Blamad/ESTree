#include "CameraBehaviour.h"

void CameraBehaviour::update(double dt, InputState &inputState) {
	while (!inputState.mousePositionQueueEmpty()) {
		std::cout << "Ruch mysza" << std::endl;
		Point2d movement = inputState.popMousePosition();
		camera->processMouseMovement(movement.x, movement.y);
	}

	while (!inputState.mouseScrollQueueEmpty()) {
		std::cout << "Ruch scrollem" << std::endl;
		Point2d movement = inputState.popMouseScroll();
		camera->processMouseScroll(movement.y);
	}
};