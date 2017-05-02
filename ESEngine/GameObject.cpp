#include "GameObject.h"

GameObject::GameObject(const Uuid &id) {
	this->id = id;
}

Component* GameObject::getComponent(ComponentType type) {
	map<ComponentType, shared_ptr<Component>>::iterator it = components.find(type);
	Component *comp = nullptr;
	if (it != components.end())
		comp = it->second.get();

	return comp;
}

void GameObject::addComponent(shared_ptr<Component> component) {
	component->setParent(*this);
	components[component->type] = component;
}