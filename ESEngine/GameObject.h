#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <map>
#include <memory>
#include <typeinfo>

#include "Component.h"

using namespace std;
typedef boost::uuids::uuid Uuid;

class GameObject {
public:
	GameObject();
	
	Component* getComponent(ComponentType componentType);
	void addComponent(shared_ptr<Component> component);

	Uuid id;
private:
	map<ComponentType, shared_ptr<Component>> components;

	static boost::uuids::random_generator uuidGenerator;
};

#endif GAMEOBJECT_H