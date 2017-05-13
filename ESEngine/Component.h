#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>
#include <iomanip>

class GameObject;

enum ComponentType {
	RENDERABLE, TRANSFORM, CAMERA, BEHAVIOUR, LIGHT
};

class Component {
public:
	ComponentType type;
	void setParent(GameObject &parent);
	Component* getComponent(const ComponentType &type);

protected:
	Component(const ComponentType &type) : type(type) {};

private:
	GameObject *parent;
};

#endif