#include "GameObject.h"

GameObject::GameObject(const Uuid &id) {
	this->id = id;
}

Component* GameObject::getComponent(ComponentType type) {
	map<ComponentType, shared_ptr<Component>>::iterator it = components.find(type);
	Component *comp = NULL;
	if (it != components.end())
		comp = it->second.get();

	return comp;
}

void GameObject::addComponent(const Component &component) {
	components[component.type] = make_shared<Component>(component);
}