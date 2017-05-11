#pragma once

#include "Behaviour.h"
#include "Transform.h"

class CubeBehaviour : public Behaviour {
public:
	void update(double dt, InputState &inputState);
private:
	Transform *transform = nullptr;
};