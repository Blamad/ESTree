#ifndef LIGHT_H
#define LIGHT_H

#include <GLM\glm.hpp>
#include <GLFW\glfw3.h>
#include "Component.h"

enum LightType {
	LIGHT_POINT, LIGHT_DIRECTIONAL, LIGHT_SPOT
};

class Light : public Component {
public:
	LightType lightType;

protected:
	Light(const LightType &lightType) : lightType(lightType), Component(LIGHT) { };
};

#endif