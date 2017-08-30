#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include <btBulletDynamicsCommon.h>
#include "RigidBody.h"

class PhysicsManager {
public:
	PhysicsManager();
	~PhysicsManager();

	void addRigidBody(RigidBody* rigidBody);
	void step(double deltaTime);

private:
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
};

#endif