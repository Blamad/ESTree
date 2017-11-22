#include "RigidBody.h"

RigidBody::RigidBody() : Component(RIGIDBODY) {
	
}

void RigidBody::update() {
	updateTransform();
}

void RigidBody::updateTransform() {
	transform->btTransform = rigidBody->getCenterOfMassTransform();
}

void RigidBody::updateRigidBody() {
	rigidBody->setCenterOfMassTransform(transform->getBtTransform());
}

void RigidBody::initAsBox(double mass, glm::vec3 boxSize) {
	this->transform = (Transform*)getComponent(TRANSFORM);
	this->mass = mass;

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
	rigidBody->setUserPointer((void*)parent);
}

void RigidBody::initAsAHullShape() {
	btCollisionShape* boxCollisionShape = new btConvexHullShape();

	//TODO ogarnij bardziej skomplikowane figury fizyczne
	//create a hull approximation
	/*btConvexHullShape* hull = new btConvexHullShape(boxCollisionShape);
	btScalar margin = boxCollisionShape->getMargin();
	hull->buildHull(margin);
	btConvexHullShape* simplifiedConvexShape = new btConvexHullShape(hull->getVertexPointer(), hull->numVertices());*/
}

void RigidBody::makeDynamic() {
	pickable = true;
	rigidBody->setActivationState(DISABLE_DEACTIVATION);
}

double RigidBody::getCurrentMass() {
	return this->mass;
}

void RigidBody::changeMass(double mass) {
	btVector3 fallInertia(0, 0, 0);
	rigidBody->getCollisionShape()->calculateLocalInertia(mass, fallInertia);
	rigidBody->setMassProps(mass, fallInertia);
	rigidBody->clearForces();

	needsReload = true;
}

void RigidBody::translate(glm::vec3 position) {
	this->transform->translate(position);
	updateRigidBody();
}

void RigidBody::rotate(float angle, glm::vec3 axis) {
	this->transform->rotate(angle, axis);
	updateRigidBody();
}