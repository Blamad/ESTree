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
	void initAsAHullShape();
	
	btRigidBody* rigidBody;
	bool pickable = false;
	bool needsReload = false;
	
	void update();

	void translate(glm::vec3 position);
	void rotate(float angle, glm::vec3 axis);

	void makeDynamic();
	double getCurrentMass();
	void changeMass(double mass);

protected:
	void updateTransform();
	void updateRigidBody();

private:
	double mass;
	Transform* transform;
};

#endif