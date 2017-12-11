#include "RigidBody.h"

Logger RigidBody::logger("RigidBody");

RigidBody::RigidBody() : Component(RIGIDBODY) {
	
}

void RigidBody::update() {
	updateTransform();
}

void RigidBody::updateTransform() {
	transform->transform = rigidBody->getCenterOfMassTransform();
	transform->updateModelMatrix();
}

void RigidBody::updateRigidBody() {
	rigidBody->setCenterOfMassTransform(transform->getBtTransform());
}

void RigidBody::initAsBox(double mass) {
	this->transform = (Transform*)getComponent(TRANSFORM);
	this->mass = mass;

	btCollisionShape* boxCollisionShape = new btBoxShape(btVector3(1, 1, 1));
	boxCollisionShape->setLocalScaling(btVector3(transform->getScale().x, transform->getScale().y, transform->getScale().z));

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

void RigidBody::initAsAHullShape(double mass) {
	this->transform = (Transform*)getComponent(TRANSFORM);
	Mesh* mesh = (Mesh*)getComponent(RENDERABLE);
	this->mass = mass;

	btCollisionShape* simplifiedConvexShape = calculateHullCollisionShape(mesh);
	simplifiedConvexShape->setLocalScaling(btVector3(transform->getScale().x, transform->getScale().y, transform->getScale().z));

	btTransform& btTransform = transform->getBtTransform();
	btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform);

	btVector3 fallInertia(0, 0, 0);
	simplifiedConvexShape->calculateLocalInertia(mass, fallInertia);

	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
		mass,
		motionstate,
		simplifiedConvexShape,
		fallInertia
	);

	rigidBody = new btRigidBody(rigidBodyCI);
	rigidBody->setUserPointer((void*)parent);
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

btCollisionShape* RigidBody::calculateHullCollisionShape(Mesh* mesh) {
	btTriangleMesh *trimesh = new btTriangleMesh();
	for (int i = 0; i < mesh->indices.size(); i += 3)
	{
		int index0 = mesh->indices[i];
		int index1 = mesh->indices[i + 1];
		int index2 = mesh->indices[i + 2];

		btVector3 vertex0(mesh->vertices[index0].position.x, mesh->vertices[index0].position.y, mesh->vertices[index0].position.z);
		btVector3 vertex1(mesh->vertices[index1].position.x, mesh->vertices[index1].position.y, mesh->vertices[index1].position.z);
		btVector3 vertex2(mesh->vertices[index2].position.x, mesh->vertices[index2].position.y, mesh->vertices[index2].position.z);

		trimesh->addTriangle(vertex0, vertex1, vertex2);
	}

	btConvexShape *collisionShape = new btConvexTriangleMeshShape(trimesh, false);
	btShapeHull *hull = new btShapeHull(collisionShape);
	btScalar margin = collisionShape->getMargin();
	hull->buildHull(margin);
	
	return new btConvexHullShape((const btScalar*)hull->getVertexPointer(), hull->numVertices());
}