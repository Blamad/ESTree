#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>
#include <iomanip>

class GameObject;

enum ComponentType {
	RENDERABLE, TRANSFORM, CAMERA, BEHAVIOUR
};

class Component {
public:
	ComponentType type;
	void setParent(GameObject &parent);

protected:
	Component(const ComponentType &type) : type(type) {};

	Component* getComponent(const ComponentType &type);

private:
	GameObject *parent;
};

#endif