#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include "Component.h"

class Transform : public Component {
public:
	Transform(const glm::vec3 &position, const glm::vec4 &rotation, const glm::vec3 &scale) : position(position), rotation(rotation), scale(scale), Component(TRANSFORM) {};

	glm::vec3 position;
	glm::vec4 rotation;
	glm::vec3 scale;
};

#endif