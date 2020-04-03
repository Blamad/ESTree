#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <map>
#include <memory>
#include <typeinfo>

class Component;
enum ComponentType;

typedef boost::uuids::uuid Uuid;

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
	static boost::uuids::random_generator uuidGenerator;
	
	std::vector<std::shared_ptr<Component>> components;
};

#endif GAMEOBJECT_H