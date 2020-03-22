#ifndef LINDENMAYER_TREE_FACTORY_H
#define LINDENMAYER_TREE_FACTORY_H

#include "GameObject.h"
#include "LindenmayerTreeParams.h"
#include "LindenmayerTree.h"
#include "SceneManager.h"
#include "Rigidbody.h"
#include "Context.h"
#include <GLM\glm.hpp>
#include <memory>

class LindenmayerTreeFactory {
public:

	static LindenmayerTreeFactory& getInstance();

	void operator=(LindenmayerTreeFactory const&) = delete;
	LindenmayerTreeFactory(LindenmayerTreeFactory const&) = delete;
	~LindenmayerTreeFactory() {}
	
	GameObject* generateTree(LindenmayerTreeParams &params, std::string &name, Material &bark, Material &leaves, glm::vec3 &position);

private:
	static LindenmayerTreeFactory factory;

	LindenmayerTreeFactory() {}

};

#endif