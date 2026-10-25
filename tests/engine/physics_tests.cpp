#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <memory>
#include <vector>

#include "Engine/GameObject/RigidBody.h"
#include "Engine/Manager/PhysicsManager.h"

namespace {

std::unique_ptr<GameObject> makeBox(double mass, float y) {
	std::unique_ptr<GameObject> go(new GameObject());
	go->getComponentAs<Transform>(TRANSFORM)->translate(glm::vec3(0, y, 0));
	auto body = std::make_shared<RigidBody>();
	go->addComponent(body);
	body->initAsBox(mass);
	return go;
}

}

// Same order as Scene::~Scene: bodies leave the world, objects die, the manager dies last.
// Under AddressSanitizer a wrong order is a heap-use-after-free inside ~btCollisionWorld.
TEST_CASE("PhysicsManager: bodies removed from the world before objects and manager are destroyed") {
	auto physics = std::make_unique<PhysicsManager>();
	std::vector<std::unique_ptr<GameObject>> objects;
	for (int i = 0; i < 10; ++i) {
		objects.push_back(makeBox(1.0, 2.0f * i));
		physics->addRigidBody(objects.back()->getComponentAs<RigidBody>(RIGIDBODY));
	}

	physics->update(0.016);
	physics->update(0.016);

	// remove one object while the world lives on
	physics->removeRigidBody(objects.front()->getComponentAs<RigidBody>(RIGIDBODY));
	objects.erase(objects.begin());
	physics->update(0.016);

	for (auto &go : objects)
		physics->removeRigidBody(go->getComponentAs<RigidBody>(RIGIDBODY));
	objects.clear();
	physics.reset();
}

TEST_CASE("PhysicsManager: removing a body that was never added, or never initialised, is harmless") {
	PhysicsManager physics;
	auto notAdded = makeBox(1.0, 0.0f);
	physics.removeRigidBody(notAdded->getComponentAs<RigidBody>(RIGIDBODY));

	RigidBody uninitialised;
	physics.removeRigidBody(&uninitialised);
	physics.removeRigidBody(nullptr);
}

TEST_CASE("RigidBody: owns its Bullet objects, destroying a body that is not in any world is fine") {
	{
		RigidBody uninitialised;
	}
	auto go = makeBox(0.0, 0.0f);
	CHECK(go->getComponentAs<RigidBody>(RIGIDBODY)->rigidBody != nullptr);
	go.reset();
}

TEST_CASE("PhysicsManager: static plane body can be created and destroyed") {
	PhysicsManager physics;
	std::unique_ptr<GameObject> floor(new GameObject());
	auto body = std::make_shared<RigidBody>();
	floor->addComponent(body);
	body->initAsAStaticPlane();
	physics.addRigidBody(body.get());
	physics.update(0.016);
	physics.removeRigidBody(body.get());
}
