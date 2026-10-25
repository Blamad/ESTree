#include "GameObject.h"
#include "Transform.h"

GameObject::GameObject() {
	this->id = Uuid::generate();
	addComponent(std::shared_ptr<Transform>(new Transform()));
}

GameObject::~GameObject() = default;

std::vector<Component*> GameObject::getComponents(ComponentType type) {
	std::vector<Component*> filteredComponents;
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

void GameObject::addComponent(std::shared_ptr<Component> component) {
	component->setParent(*this);
	components.push_back(component);
}

void GameObject::addGameObject(std::shared_ptr<GameObject> go) {
	this->children.push_back(go);
	go->parent = this;
}