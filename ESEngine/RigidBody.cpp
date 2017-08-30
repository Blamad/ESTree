#include "RigidBody.h"

RigidBody::RigidBody() : Component(RIGIDBODY) {
	
}

void RigidBody::updateTransform() {
	rigidBody->getMotionState()->getWorldTransform(transform->getBtTransform());
}

void RigidBody::initAsBox(double mass, glm::vec3 boxSize) {
	transform = (Transform*)getComponent(TRANSFORM);

	btCollisionShape* boxCollisionShape = new btBoxShape(btVector3(boxSize.x, boxSize.y, boxSize.z));

	btTransform& btTransform = transform->getBtTransform();
	btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform);

	btVector3 fallInertia(0, 0, 0);
	boxCollisionShape->calculateLocalInertia(mass, fallInertia);

	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
		mass,
		motionstate,
		boxCollisionShape,
		fallInertia
	);
	rigidBody = new btRigidBody(rigidBodyCI);
	rigidBody->setUserPointer((void*)this);
}
