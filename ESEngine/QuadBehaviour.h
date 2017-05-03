#pragma once

#include "Behaviour.h"
#include "Transform.h"

class QuadBehaviour : public Behaviour {
public:
	void update(double dt, InputState &inputState);
private:
	Transform *transform = nullptr;
};