#include "Transform.h"

glm::mat4 Transform::getModelMatrix() {
	if (modelValueChanged) {
		modelMatrix = glm::mat4();
		modelMatrix = glm::translate(modelMatrix, getPosition());
		modelMatrix *= mat4_cast(getRotation());
		modelMatrix = glm::scale(modelMatrix, scaleVec);

		modelValueChanged = false;
		updateNormalModelMatrix(modelMatrix);
	}

	if (parent->parent != nullptr) {
		Transform* pTransform = (Transform*)parent->parent->getComponent(TRANSFORM);
		return pTransform->getModelMatrix() * modelMatrix;
	}

	return modelMatrix;
}

glm::mat3 Transform::getNormalModelMatrix() {

	glm::mat4 model(1.0f);
	model *= mat4_cast(getRotation());

	normalModelMatrix = glm::mat3(model);

	if (parent->parent != nullptr) {
		Transform* pTransform = (Transform*)parent->parent->getComponent(TRANSFORM);
		normalModelMatrix = pTransform->getNormalModelMatrix() * normalModelMatrix;
	}

	return normalModelMatrix;
}

void Transform::updateNormalModelMatrix(glm::mat4 modelMatrix) {
	normalModelMatrix = glm::mat3(modelMatrix);
	if (parent->parent != nullptr) {
		Transform* pTransform = (Transform*)parent->parent->getComponent(TRANSFORM);
		normalModelMatrix = pTransform->getNormalModelMatrix() * normalModelMatrix;
	}
}

void Transform::setModelMatrix(glm::mat4 modelMatrix) {
	this->modelMatrix = modelMatrix;
	updateNormalModelMatrix(modelMatrix);
	modelValueChanged = false;
}

glm::vec3 Transform::getPosition() {
	return bulletToGlm(transform.getOrigin());
}

glm::vec3 Transform::getScale() {
	return scaleVec;
}

glm::quat Transform::getRotation() {
	return bulletToGlm(transform.getRotation());
}

void Transform::translate(glm::vec3 position) {
	modelValueChanged = true;
	this->transform.setOrigin(transform.getOrigin() + glmToBullet(position));
}

void Transform::rotate(glm::quat rotation) {
	modelValueChanged = true;
	transform.setRotation(transform.getRotation() * glmToBullet(rotation));
}

void Transform::rotate(float angle, glm::vec3 axis) {
	modelValueChanged = true;
	transform.setRotation(transform.getRotation() * btQuaternion(glmToBullet(axis), angle));
}

void Transform::scale(glm::vec3 scale) {
	modelValueChanged = true;
	this->scaleVec *= scale;
}

void Transform::clearRotation() {
	modelValueChanged = true;
	transform.setRotation(btQuaternion::getIdentity());
}

btTransform& Transform::getBtTransform() {
	return transform;
}

void Transform::updateModelMatrix() {
	modelValueChanged = true;
}