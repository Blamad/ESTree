#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <btBulletDynamicsCommon.h>
#include <GLM\glm.hpp>

#include "Component.h"
#include "Transform.h"

class RigidBody : public Component {
public:
	RigidBody();
	void initAsBox(double mass, glm::vec3 boxSize);
	btRigidBody* rigidBody;

	void updateTransform();
private:
	Transform* transform;
};

#endif