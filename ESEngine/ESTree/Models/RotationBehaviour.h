#pragma once
#include "Engine/GameObject/Behaviour.h"
#include "Engine/GameObject/Transform.h"
#include "Engine/GameObject/RigidBody.h"

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
			rigidBody->rotate(glm::radians(angle), glm::vec3(0, 1, 0));
		}
		else {
			transform->clearRotation();
			transform->rotate(angle, glm::vec3(0, 1, 0));
			transform->rotate(glm::radians(45.0f), glm::vec3(1, 0, 1));
		}
	}

private:
	glm::vec4 baseRotation;
	double angle = 0;
	Transform *transform = nullptr;
	RigidBody *rigidBody = nullptr;
};