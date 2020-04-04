#include "TreeBehaviour.h"

void TreeBehaviour::update(double dt, InputState &inputState) {
	if (transform == nullptr || mesh == nullptr) {
		transform = (Transform*)getComponent(TRANSFORM);
		mesh = (Mesh*)getComponent(RENDERABLE);
	}

	if (inputState.isKeyPressed(GLFW_KEY_I)) {

	}

	if (inputState.isKeyPressed(GLFW_KEY_K)) {

	}
}