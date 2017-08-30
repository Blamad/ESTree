#include "GameObject.h"
#include "Transform.h"

boost::uuids::random_generator GameObject::uuidGenerator;

GameObject::GameObject() {
	this->id = uuidGenerator();
	addComponent(shared_ptr<Transform>(new Transform()));
}

vector<Component*> GameObject::getComponents(ComponentType type) {
	vector<Component*> filteredComponents;
	for (auto & comp : components) {
		if(comp->type == type)
			filteredComponents.push_back(comp.get());
	}
	return filteredComponents;
}

Component* GameObject::getComponent(ComponentType type) {
	Component* component = nullptr;
	for (auto & comp : components) {
		if (comp->type == type)
			component = comp.get();
	}
	return component;
}

void GameObject::addComponent(shared_ptr<Component> component) {
	component->setParent(*this);
	components.push_back(component);
}