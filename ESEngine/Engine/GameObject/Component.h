#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>
#include <iostream>
#include <iomanip>
#include "GameObject.h"

enum ComponentType {
	RENDERABLE, TRANSFORM, CAMERA, BEHAVIOUR, LIGHT, RIGIDBODY
};

class Component {
public:
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

protected:
	Component(const ComponentType &type) : type(type) {};
	GameObject *parent = nullptr;
};

#endif