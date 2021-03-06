#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include <btBulletDynamicsCommon.h>

#include <boost/uuid/uuid_io.hpp>

#include "Engine/GameObject/RigidBody.h"
#include "Ray.h"
#include "Context.h"
#include "Engine/UI/ConsoleMemory.h"

#include "Engine/Core/Logger.h"

class PhysicsManager {
public:
	PhysicsManager();
	~PhysicsManager();

	void addRigidBody(RigidBody* rigidBody);
	void reloadRigidBody(RigidBody* rigidBody);
	void removeRigidBody(RigidBody * rigidBody);
	void update(double deltaTime);
	GameObject* raycast(Ray &ray);

private:
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	static Logger logger;
};

#endif