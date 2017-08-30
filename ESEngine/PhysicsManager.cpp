#include "PhysicsManager.h"

PhysicsManager::PhysicsManager() {
	// Build the broadphase
	broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	solver = new btSequentialImpulseConstraintSolver;

	// The world.
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

PhysicsManager::~PhysicsManager() {
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;
}

void PhysicsManager::addRigidBody(RigidBody* rigidBody) {
	dynamicsWorld->addRigidBody(rigidBody->rigidBody);
}

void PhysicsManager::step(double deltaTime) {
	dynamicsWorld->stepSimulation(deltaTime, 10);
}