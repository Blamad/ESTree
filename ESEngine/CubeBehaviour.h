#pragma once

#include "Behaviour.h"
#include "Transform.h"

using namespace glm;

class CubeBehaviour : public Behaviour {
public:
	void update(double dt, InputState &inputState);
private:
	vec4 baseRotation;
	double angle = 0;
	Transform *transform = nullptr;
};