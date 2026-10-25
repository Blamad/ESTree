#ifndef LINDENMAYER_TREE_FACTORY_H
#define LINDENMAYER_TREE_FACTORY_H

#include <cstdint>
#include <memory>
#include <string>

#include <glm/glm.hpp>
#include <treegen/TreeParams.h>

#include "Engine/GameObject/GameObject.h"
#include "LindenmayerTree.h"
#include "Engine/Manager/SceneManager.h"
#include "Engine/GameObject/RigidBody.h"
#include "Engine/Manager/Context.h"

class LindenmayerTreeFactory {
public:

	static LindenmayerTreeFactory& getInstance();

	void operator=(LindenmayerTreeFactory const&) = delete;
	LindenmayerTreeFactory(LindenmayerTreeFactory const&) = delete;
	~LindenmayerTreeFactory() {}

	// Generates the tree, gives it a hull RigidBody and adds it to `scene`, or to the active scene when `scene` is null
	// (a scene that is still being constructed must pass itself: it is not the active scene yet).
	// `seed` is the console seed (0 = none), see LindenmayerTree. Throws treegen::Error (RewriteError, InterpretError)
	// and std::length_error; nothing is added to the scene then.
	LindenmayerTree* generateTree(const treegen::TreeParams &params, const std::string &name, const Material &bark, const Material &leaves, const glm::vec3 &position, uint32_t seed = 0, Scene *scene = nullptr, bool meshWiring = false, bool normalVisualisation = false);

	// Loads the rule file and generates the tree. A missing or malformed file or a failing generation (LoadError,
	// RewriteError, InterpretError) is logged and gives nullptr: a scene that loads files at startup skips that tree.
	LindenmayerTree* generateTreeFromFile(const std::string &path, const std::string &name, const Material &bark, const Material &leaves, const glm::vec3 &position, uint32_t seed = 0, Scene *scene = nullptr, bool meshWiring = false, bool normalVisualisation = false);

private:
	static LindenmayerTreeFactory factory;

	LindenmayerTreeFactory() {}

};

#endif
