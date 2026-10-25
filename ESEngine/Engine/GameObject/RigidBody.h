#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

#include <glm/glm.hpp>

#include "Engine/Core/Logger.h"
#include "Transform.h"
#include "Mesh.h"

class RigidBody : public Component {
public:
	RigidBody();
	// Owns the Bullet body, its motion state and its shape. The body must already have been
	// removed from the PhysicsManager's world (Scene does that before destroying a game object).
	~RigidBody();
	RigidBody(const RigidBody&) = delete;
	RigidBody& operator=(const RigidBody&) = delete;

	void initAsBox(double mass);
	void initAsAHullShape(double mass);
	void initAsAStaticPlane();
	
	btRigidBody* rigidBody = nullptr;
	bool pickable = false;
	//marked when we update rigidbody position externally
	bool needsReload = false;
	
	void update();

	void translate(glm::vec3 position);
	void rotate(float angle, glm::vec3 axis);

	void makeDynamic();
	double getCurrentMass();
	void clearForces();
	void changeMass(double mass);

protected:
	void updateTransform();
	void updateRigidBody();

private:
	static Logger logger;

	double mass;
	Transform* transform = nullptr;

	btCollisionShape* calculateHullCollisionShape(Mesh* mesh);
};

#endif