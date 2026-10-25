#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Uuid.h"
#include <map>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

class Component;
enum ComponentType;

class GameObject {
public:
	GameObject();
	
	std::vector<Component*> getComponents(ComponentType componentType);
	Component* getComponent(ComponentType componentType);
	void addComponent(std::shared_ptr<Component> component);

	void addGameObject(std::shared_ptr<GameObject> go);

	Uuid id;
	std::string name = "";

	GameObject* parent = nullptr;
	std::vector<std::shared_ptr<GameObject>> children;

private:
	std::vector<std::shared_ptr<Component>> components;
};

#endif GAMEOBJECT_H