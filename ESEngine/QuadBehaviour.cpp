#include "QuadBehaviour.h"

void QuadBehaviour::update(double dt, InputState &inputState) {
	if (transform == nullptr)
		transform = (Transform*)getComponent(TRANSFORM);
	transform->rotation.w += dt;
}