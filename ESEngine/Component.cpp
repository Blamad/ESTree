#include "Component.h"
#include "GameObject.h"

void Component::setParent(GameObject &parent) {
	this->parent = &parent; 
}

Component* Component::getComponent(const ComponentType &type) {
	return parent->getComponent(type); 
}