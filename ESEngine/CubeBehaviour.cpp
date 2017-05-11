#include "CubeBehaviour.h"

void CubeBehaviour::update(double dt, InputState &inputState) {
	if (transform == nullptr)
		transform = (Transform*)getComponent(TRANSFORM);
	transform->rotation.w += dt;

	for (Point2d const& scrollVal : inputState.getMouseScrollEvents()) {
		transform->scale += 0.1*scrollVal.y;
		float x = transform->scale.x;
		cout << "scale: " << x << endl;
	}
}