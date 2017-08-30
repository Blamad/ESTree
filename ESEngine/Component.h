#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>
#include <iostream>
#include <iomanip>
#include "GameObject.h"

using namespace std;

enum ComponentType {
	RENDERABLE, TRANSFORM, CAMERA, BEHAVIOUR, LIGHT, RIGIDBODY
};

class Component {
public:
	ComponentType type;

	void setParent(GameObject &parent) {
		this->parent = &parent;
	}

	vector<Component*> getComponents(const ComponentType &type) {
		return parent->getComponents(type);
	}

	Component* getComponent(const ComponentType &type) {
		return parent->getComponent(type);
	}

protected:
	Component(const ComponentType &type) : type(type) {};

private:
	GameObject *parent = nullptr;
};

#endif