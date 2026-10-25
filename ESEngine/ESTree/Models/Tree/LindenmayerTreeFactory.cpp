#include "LindenmayerTreeFactory.h"

#include <stdexcept>

#include <treegen/Error.h>

#include "Engine/Core/Logger.h"

static Logger logger("LindenmayerTreeFactory");

LindenmayerTreeFactory& LindenmayerTreeFactory::getInstance() {
	static LindenmayerTreeFactory factory;
	return factory;
}

LindenmayerTree* LindenmayerTreeFactory::generateTree(const treegen::TreeParams &params, const std::string &name, const Material &bark, const Material &leaves, const glm::vec3 &position, uint32_t seed, Scene *scene, bool meshWiring, bool normalVisualisation) {
	std::unique_ptr<LindenmayerTree> tree(new LindenmayerTree(params, bark, leaves, meshWiring, normalVisualisation, seed));
	tree->name = name;
	Transform *transform = tree->getComponentAs<Transform>(TRANSFORM);
	transform->translate(position);
	tree->generate();

	// The hull is built from the mesh that generate() filled, so it sees the final vertices.
	Mesh *mesh = tree->getComponentAs<Mesh>(RENDERABLE);
	if (mesh != nullptr && !mesh->indices.empty()) {
		std::shared_ptr<RigidBody> rigidBody = std::shared_ptr<RigidBody>(new RigidBody());
		tree->addComponent(rigidBody);
		rigidBody->initAsAHullShape(0);
	}
	else {
		logger.log(LOG_WARN, "Tree " + name + " has no geometry, no collision shape created");
	}

	LindenmayerTree *treePtr = tree.get();
	if (scene != nullptr)
		scene->addGameObject(std::move(tree));
	else
		Context::getSceneManager()->addGameObject(std::move(tree));
	return treePtr;
}

LindenmayerTree* LindenmayerTreeFactory::generateTreeFromFile(const std::string &path, const std::string &name, const Material &bark, const Material &leaves, const glm::vec3 &position, uint32_t seed, Scene *scene, bool meshWiring, bool normalVisualisation) {
	try {
		return generateTree(treegen::loadTreeParams(path), name, bark, leaves, position, seed, scene, meshWiring, normalVisualisation);
	}
	catch (const treegen::Error &error) {
		logger.log(LOG_ERROR, "Tree " + name + " skipped: " + error.what());
	}
	catch (const std::length_error &error) {
		logger.log(LOG_ERROR, "Tree " + name + " skipped: " + error.what());
	}
	return nullptr;
}
