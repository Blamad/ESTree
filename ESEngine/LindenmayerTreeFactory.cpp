#include "LindenmayerTreeFactory.h"

LindenmayerTreeFactory& LindenmayerTreeFactory::getInstance() {
	static LindenmayerTreeFactory factory;
	return factory;
}

GameObject* LindenmayerTreeFactory::generateTree(LindenmayerTreeParams &params, std::string &name, Material &bark, Material &leaves, glm::vec3 &position) {
	std::unique_ptr<GameObject> go = std::unique_ptr<GameObject>(new LindenmayerTree(params, bark, leaves, false, false));
	go->name = name;
	Transform *transform = (Transform*)go->getComponent(TRANSFORM);
	transform->translate(position);
	((LindenmayerTree*)go.get())->generate();

	std::shared_ptr<RigidBody> rigidBody = std::shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsAHullShape(0);
	GameObject *objectPtr = go.get();

	Context::getSceneManager()->addGameObject(move(go));
	return objectPtr;
}