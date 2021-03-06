#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <btBulletDynamicsCommon.h>
#include "Component.h"

class RigidBody;

class Transform : public Component {
public:
	Transform() : Component(TRANSFORM) {};
	
	glm::mat4 getModelMatrix();
	glm::mat3 getNormalModelMatrix();
	void setModelMatrix(glm::mat4 modelMatrix);
	glm::vec3 getPosition();
	glm::quat getRotation();
	glm::vec3 getScale();

	btTransform& getBtTransform();
	void updateModelMatrix();

	btTransform transform = btTransform::getIdentity();


	void rotate(float angle, glm::vec3 axis);
	void translate(glm::vec3 position);
	void rotate(glm::quat rotation);
	void scale(glm::vec3 scale);
	void clearRotation();

private:
	glm::vec3 scaleVec = glm::vec3(1, 1, 1);
	glm::mat4 modelMatrix = glm::mat4(1);
	glm::mat3 normalModelMatrix = glm::mat3(1);

	bool modelValueChanged = true;

	void updateNormalModelMatrix(glm::mat4 modelMatrix);

	glm::vec3 bulletToGlm(const btVector3& v) {
		glm::vec3 vec(v.getX(), v.getY(), v.getZ());
		return vec; 
	}

	btVector3 glmToBullet(const glm::vec3& v) {
		btVector3 vec(v.x, v.y, v.z);
		return vec; 
	}

	glm::quat bulletToGlm(const btQuaternion& q) {
		glm::quat quat(q.getW(), q.getX(), q.getY(), q.getZ());
		return quat; 
	}

	btQuaternion glmToBullet(const glm::quat& q) { 
		btQuaternion quat(q.x, q.y, q.z, q.w); 
		return quat;
	}
};

#endif