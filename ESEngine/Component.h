#ifndef COMPONENT_H
#define COMPONENT_H

enum ComponentType {
	RENDERABLE, TRANSFORM, CAMERA, BEHAVIOUR
};

class Component {
public:
	ComponentType type;

protected:
	Component(const ComponentType &type) : type(type) {};
};

#endif