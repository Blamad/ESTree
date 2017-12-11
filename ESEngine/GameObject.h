#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <map>
#include <memory>
#include <typeinfo>

class Component;
enum ComponentType;

using namespace std;
typedef boost::uuids::uuid Uuid;

class GameObject {
public:
	GameObject();
	
	vector<Component*> getComponents(ComponentType componentType);
	Component* getComponent(ComponentType componentType);
	void addComponent(shared_ptr<Component> component);

	void addGameObject(shared_ptr<GameObject> go);

	Uuid id;
	string name = "";

	GameObject* parent = nullptr;
	vector<shared_ptr<GameObject>> children;

private:
	static boost::uuids::random_generator uuidGenerator;
	
	vector<shared_ptr<Component>> components;
};

#endif GAMEOBJECT_H