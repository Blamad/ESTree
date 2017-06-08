#pragma once
#include "Behaviour.h"
#include "Transform.h"
#include "Mesh.h"

#include <GLFW\glfw3.h>

using namespace glm;

class TreeBehaviour : public Behaviour {
public:

	void update(double dt, InputState &inputState);

private:
	Mesh *mesh = nullptr;
	Transform *transform = nullptr;
};