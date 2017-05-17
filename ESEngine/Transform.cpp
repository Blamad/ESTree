#include "Transform.h"

glm::mat4 Transform::getModelMatrix() {
	glm::mat4 model;
	model = glm::translate(model, positionVec);
	model *= rotationMatrix;
	model = glm::scale(model, scaleVec);

	return model;
}

glm::mat3 Transform::getNormalModelMatrix() {
	glm::mat4 model(1.0f);
	model *= rotationMatrix;
	model = glm::scale(model, scaleVec);
	model = glm::transpose(glm::inverse(model));

	return glm::mat3(model);
}

glm::vec3 Transform::getPosition() {
	return positionVec;
}

glm::vec3 Transform::getScale() {
	return scaleVec;
}

void Transform::translate(glm::vec3 position) {
	this->positionVec += position;
}

void Transform::rotate(float angle, glm::vec3 axis) {
	rotationMatrix = glm::rotate(rotationMatrix, angle, axis);
}

void Transform::scale(glm::vec3 scale) {
	this->scaleVec *= scale;
}

void Transform::clearRotation() {
	rotationMatrix = glm::mat4();
};