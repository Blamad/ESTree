#ifndef LIGHT_H
#define LIGHT_H

#include <GLM\glm.hpp>
#include <GLFW\glfw3.h>
#include "Component.h"

using namespace glm;

enum LightType {
	POINT, DIRECTIONAL, SPOT
};

class Light : public Component {
public:
	LightType lightType;

protected:
	Light(const LightType &lightType) : lightType(lightType), Component(LIGHT) { };
};

#endif