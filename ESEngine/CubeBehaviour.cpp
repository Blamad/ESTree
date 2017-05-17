#include "CubeBehaviour.h"

void CubeBehaviour::update(double dt, InputState &inputState) {
	if (transform == nullptr) {
		transform = (Transform*)getComponent(TRANSFORM);
		angle = 0;
	}

	angle += dt;

	transform->clearRotation();
	transform->rotate(angle, vec3(0, 1, 0));
	transform->rotate(radians(45.0f), vec3(1, 0, 1));

	for (Point2d const& scrollVal : inputState.getMouseScrollEvents()) {
		transform->scale(vec3(1 + 0.1*scrollVal.y));
	}
}