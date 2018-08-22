#include "LindenmayerTreeFactory.h"

LindenmayerTreeFactory& LindenmayerTreeFactory::getInstance() {
	static LindenmayerTreeFactory factory;
	return factory;
}

GameObject* LindenmayerTreeFactory::generateTree(LindenmayerTreeParams &params, string &name, Material &bark, Material &leaves, vec3 &position) {
	unique_ptr<GameObject> go = unique_ptr<GameObject>(new LindenmayerTree(params, bark, leaves, false, false));
	go->name = name;
	Transform *transform = (Transform*)go->getComponent(TRANSFORM);
	transform->translate(position);
	((LindenmayerTree*)go.get())->generate();

	shared_ptr<RigidBody> rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsAHullShape(0);
	GameObject *objectPtr = go.get();

	Context::getSceneManager()->addGameObject(move(go));
	return objectPtr;
}