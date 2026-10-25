#ifndef COMPONENT_H
#define COMPONENT_H

#include <cassert>
#include <type_traits>
#include <vector>
#include <iostream>
#include <iomanip>
#include "GameObject.h"

enum ComponentType : int {
	RENDERABLE, TRANSFORM, CAMERA, BEHAVIOUR, LIGHT, RIGIDBODY
};

class Component {
public:
	virtual ~Component() = default;

	ComponentType type;

	void setParent(GameObject &parent) {
		this->parent = &parent;
	}

	std::vector<Component*> getComponents(const ComponentType &type) {
		return parent->getComponents(type);
	}

	Component* getComponent(const ComponentType &type) {
		return parent->getComponent(type);
	}

	template<class T> T* getComponentAs(const ComponentType &type) {
		return parent->getComponentAs<T>(type);
	}

protected:
	Component(const ComponentType &type) : type(type) {};
	GameObject *parent = nullptr;
};

template<class T>
T* GameObject::getComponentAs(ComponentType componentType) {
	static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
	Component *component = getComponent(componentType);
	assert(component == nullptr || dynamic_cast<T*>(component) != nullptr);
	return static_cast<T*>(component);
}

template<class T>
std::vector<T*> GameObject::getComponentsAs(ComponentType componentType) {
	static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
	std::vector<T*> typed;
	for (Component *component : getComponents(componentType)) {
		assert(dynamic_cast<T*>(component) != nullptr);
		typed.push_back(static_cast<T*>(component));
	}
	return typed;
}

#endif