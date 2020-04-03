#include "PhysicsManager.h"

Logger PhysicsManager::logger("PhysicsManager");

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
	
}

void PhysicsManager::addRigidBody(RigidBody *rigidBody) {
	dynamicsWorld->addRigidBody(rigidBody->rigidBody);
}

void PhysicsManager::reloadRigidBody(RigidBody *rigidBody) {
	dynamicsWorld->removeRigidBody(rigidBody->rigidBody);
	dynamicsWorld->addRigidBody(rigidBody->rigidBody);
}

void PhysicsManager::removeRigidBody(RigidBody *rigidBody) {
	dynamicsWorld->removeRigidBody(rigidBody->rigidBody);
}

void PhysicsManager::update(double deltaTime) {
	dynamicsWorld->stepSimulation(btScalar(deltaTime), 10);
}

GameObject* PhysicsManager::raycast(Ray &ray) {
	btCollisionWorld::ClosestRayResultCallback RayCallback(
		btVector3(ray.origin.x, ray.origin.y, ray.origin.z),
		btVector3(ray.end.x, ray.end.y, ray.end.z)
	);
	dynamicsWorld->rayTest(
		btVector3(ray.origin.x, ray.origin.y, ray.origin.z),
		btVector3(ray.end.x, ray.end.y, ray.end.z),
		RayCallback
	);

	if (RayCallback.hasHit()) {
		GameObject *go = static_cast<GameObject*>(RayCallback.m_collisionObject->getUserPointer());
		std::string name = go->name.length() > 0 ? go->name : boost::uuids::to_string(go->id);
		
		logger.log(LOG_INFO, "Object clicked: " + name);
		Context::getConsoleMemory()->pushLine("Object clicked: " + name);

		return go;
	}
	else {
		logger.log(LOG_INFO, "Click missed");
		return nullptr;
	}
}