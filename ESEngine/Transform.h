#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Component.h"

class Transform : public Component {
public:
	Transform() : Component(TRANSFORM) {};
	
	glm::mat4 getModelMatrix();
	glm::mat3 getNormalModelMatrix();
	glm::vec3 getPosition();
	glm::vec3 getScale();

	void rotate(float angle, glm::vec3 axis);
	void translate(glm::vec3 position);
	void scale(glm::vec3 scale);
	void clearRotation();

private:
	glm::vec3 positionVec;
	glm::mat4 rotationMatrix;
	glm::vec3 scaleVec = glm::vec3(1, 1, 1);
};

#endif