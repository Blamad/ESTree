#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <boost/uuid/uuid.hpp>
#include <map>
#include <memory>

#include "Component.h"

using namespace std;
typedef boost::uuids::uuid Uuid;

class GameObject {
public:
	GameObject(const Uuid &id);

	/*template< class T, class std::enable_if<std::is_base_of<MyClass, T>::value>::type* = nullptr>>
	T* getComponent<T>();*/
	Component* getComponent(ComponentType componentType);
	void addComponent(const Component &component);

	Uuid id;
private:
	map<ComponentType, shared_ptr<Component>> components;
};

#endif GAMEOBJECT_H