#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <memory>
#include <set>

#include "Engine/GameObject/Component.h"
#include "Engine/GameObject/Transform.h"

namespace {

// Minimal component: counts how many instances are alive.
struct Probe : Component {
	Probe(ComponentType type, int tag, int *alive) : Component(type), tag(tag), alive(alive) { ++*alive; }
	~Probe() { --*alive; }
	int tag;
	int *alive;
};

// Derived object with its own destructor side effect, as Cube/LindenmayerTree are in the game.
struct Derived : GameObject {
	explicit Derived(int *destroyed) : destroyed(destroyed) {}
	~Derived() { ++*destroyed; }
	int *destroyed;
};

}

TEST_CASE("GameObject: a new object has exactly one Transform and nothing else") {
	GameObject go;

	CHECK(go.getComponents(TRANSFORM).size() == 1);
	CHECK(go.getComponent(TRANSFORM) != nullptr);
	for (ComponentType other : { RENDERABLE, CAMERA, BEHAVIOUR, LIGHT, RIGIDBODY }) {
		CHECK(go.getComponent(other) == nullptr);
		CHECK(go.getComponents(other).empty());
		CHECK(go.getComponentAs<Component>(other) == nullptr);
	}
}

TEST_CASE("GameObject: getComponentAs returns the typed component or nullptr") {
	GameObject go;

	Transform *transform = go.getComponentAs<Transform>(TRANSFORM);
	REQUIRE(transform != nullptr);
	CHECK(static_cast<Component*>(transform) == go.getComponent(TRANSFORM));
	CHECK(go.getComponentAs<Transform>(CAMERA) == nullptr);
}

TEST_CASE("GameObject: addComponent sets the parent so a component can reach its siblings") {
	GameObject go;
	int alive = 0;
	auto probe = std::make_shared<Probe>(BEHAVIOUR, 1, &alive);

	go.addComponent(probe);

	CHECK(probe->getComponent(TRANSFORM) == go.getComponent(TRANSFORM));
	CHECK(probe->getComponentAs<Transform>(TRANSFORM) == go.getComponentAs<Transform>(TRANSFORM));
}

TEST_CASE("GameObject: several components of one type - getComponent returns the last one added") {
	GameObject go;
	int alive = 0;
	go.addComponent(std::make_shared<Probe>(BEHAVIOUR, 1, &alive));
	go.addComponent(std::make_shared<Probe>(BEHAVIOUR, 2, &alive));
	go.addComponent(std::make_shared<Probe>(LIGHT, 3, &alive));

	std::vector<Probe*> behaviours = go.getComponentsAs<Probe>(BEHAVIOUR);
	REQUIRE(behaviours.size() == 2);
	CHECK(behaviours[0]->tag == 1);
	CHECK(behaviours[1]->tag == 2);
	CHECK(go.getComponentAs<Probe>(BEHAVIOUR)->tag == 2);
	CHECK(go.getComponentAs<Probe>(LIGHT)->tag == 3);
}

TEST_CASE("GameObject: destroying an object destroys its components") {
	int alive = 0;
	{
		GameObject go;
		go.addComponent(std::make_shared<Probe>(BEHAVIOUR, 1, &alive));
		go.addComponent(std::make_shared<Probe>(CAMERA, 2, &alive));
		CHECK(alive == 2);
	}
	CHECK(alive == 0);
}

TEST_CASE("GameObject: deleting a derived object through a GameObject pointer runs the derived destructor") {
	// Scene owns std::unique_ptr<GameObject> that actually hold Cube/LindenmayerTree objects.
	int destroyed = 0;
	int alive = 0;
	{
		std::unique_ptr<GameObject> go(new Derived(&destroyed));
		go->addComponent(std::make_shared<Probe>(BEHAVIOUR, 1, &alive));
		CHECK(alive == 1);
	}
	CHECK(destroyed == 1);
	CHECK(alive == 0);
}

TEST_CASE("GameObject: children are owned by their parent and know it") {
	int destroyed = 0;
	{
		GameObject parent;
		auto child = std::make_shared<Derived>(&destroyed);
		parent.addGameObject(child);

		CHECK(child->parent == &parent);
		REQUIRE(parent.children.size() == 1);
		CHECK(parent.children[0].get() == child.get());
		CHECK(destroyed == 0);
	}
	CHECK(destroyed == 1);
}

TEST_CASE("GameObject: ids are unique") {
	std::set<Uuid> ids;
	for (int i = 0; i < 1000; ++i) {
		GameObject go;
		ids.insert(go.id);
	}
	CHECK(ids.size() == 1000);
}
