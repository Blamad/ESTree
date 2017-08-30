#include "Transform.h"

glm::mat4 Transform::getModelMatrix() {
	glm::mat4 model;
	model = glm::translate(model, getPosition());
	model *= mat4_cast(getRotation());
	model = glm::scale(model, scaleVec);

	return model;
}

glm::mat3 Transform::getNormalModelMatrix() {
	glm::mat4 model(1.0f);
	model *= mat4_cast(getRotation());
	model = glm::scale(model, scaleVec);
	model = glm::transpose(glm::inverse(model));

	glm::mat3 normalModel(model);
	return normalModel;
}

glm::vec3 Transform::getPosition() {
	return bulletToGlm(btTransform.getOrigin());
}

glm::vec3 Transform::getScale() {
	return scaleVec;
}

glm::quat Transform::getRotation() {
	return bulletToGlm(btTransform.getRotation());
}

void Transform::translate(glm::vec3 position) {
	this->btTransform.setOrigin(btTransform.getOrigin() + glmToBullet(position));
}

void Transform::rotate(float angle, glm::vec3 axis) {
	btTransform.setRotation(btTransform.getRotation() * btQuaternion(glmToBullet(axis), angle));
}

void Transform::scale(glm::vec3 scale) {
	this->scaleVec *= scale;
}

void Transform::clearRotation() {
	btTransform.setRotation(btQuaternion::getIdentity());
}

btTransform& Transform::getBtTransform() {
	return btTransform;
}