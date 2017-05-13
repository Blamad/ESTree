#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Component.h"

class Transform : public Component {
public:
	Transform(glm::vec3 position = glm::vec3(.0f, .0f, .0f), glm::vec4 rotation = glm::vec4(1.0f, 1.0f, 1.0f, .0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)) : position(position), rotation(rotation), scale(scale), Component(TRANSFORM) {};
	
	glm::vec3 position;
	glm::vec4 rotation;
	glm::vec3 scale;

	glm::mat4 getModelMatrix() {
		glm::mat4 model;
		model = glm::translate(model, position);
		model = glm::rotate(model, rotation.w, glm::vec3(rotation));
		model = glm::scale(model, scale);

		return model;
	};
};

#endif