#pragma once
#include "Behaviour.h"
#include "Transform.h"
#include "RigidBody.h"

using namespace glm;

class RotationBehaviour : public Behaviour {
public:

	void update(double dt, InputState &inputState) {
		if (transform == nullptr) {
			transform = (Transform*)getComponent(TRANSFORM);
			angle = 0;
		}
		if (rigidBody == nullptr) {
			rigidBody = (RigidBody*)getComponent(RIGIDBODY);
		}

		angle += dt*0.01;
		angle = fmod(angle, 360);
		if (rigidBody != nullptr) {
			rigidBody->rotate(radians(angle), vec3(0, 1, 0));
		}
		else {
			transform->clearRotation();
			transform->rotate(angle, vec3(0, 1, 0));
			transform->rotate(radians(45.0f), vec3(1, 0, 1));
		}
	}

private:
	vec4 baseRotation;
	double angle = 0;
	Transform *transform = nullptr;
	RigidBody *rigidBody = nullptr;
};