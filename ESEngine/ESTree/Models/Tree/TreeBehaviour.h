#pragma once
#include "Engine/GameObject/Behaviour.h"
#include "Engine/GameObject/Transform.h"
#include "Engine/GameObject/Mesh.h"

#include <GLFW\glfw3.h>

class TreeBehaviour : public Behaviour {
public:

	void update(double dt, InputState &inputState);

private:
	Mesh *mesh = nullptr;
	Transform *transform = nullptr;
};