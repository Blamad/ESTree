#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <map>
#include <memory>

#include "Component.h"

using namespace std;
typedef boost::uuids::uuid Uuid;

class GameObject {
public:
	GameObject();

	/*template< class T, class std::enable_if<std::is_base_of<MyClass, T>::value>::type* = nullptr>>
	T* getComponent<T>();*/
	Component* getComponent(ComponentType componentType);
	void addComponent(shared_ptr<Component> component);

	Uuid id;
private:
	map<ComponentType, shared_ptr<Component>> components;

	static boost::uuids::random_generator uuidGenerator;
};

#endif GAMEOBJECT_H