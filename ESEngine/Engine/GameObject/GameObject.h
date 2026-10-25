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
	virtual ~GameObject();

	std::vector<Component*> getComponents(ComponentType componentType);
	Component* getComponent(ComponentType componentType);

	// Typed access: T must be the concrete class registered under componentType.
	// Defined in Component.h (needs the complete Component type).
	template<class T> T* getComponentAs(ComponentType componentType);
	template<class T> std::vector<T*> getComponentsAs(ComponentType componentType);
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