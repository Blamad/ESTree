#ifndef LINDENMAYER_TREE_H
#define LINDENMAYER_TREE_H

#include <cstdint>
#include <memory>
#include <vector>

#include <treegen/TreeGenerator.h>

#include "Engine/GameObject/GameObject.h"
#include "Engine/GameObject/InstancedMesh.h"
#include "Engine/GameObject/Material.h"
#include "Engine/GameObject/Mesh.h"
#include "Engine/Manager/ShaderManager.h"

#include "Engine/Core/Logger.h"

// Engine adapter over treegen: generate() runs treegen::generateTree and uploads the result into a Mesh (trunk)
// and an InstancedMesh (leaves, in a child object). The GL buffers are sized from the generated data.
class LindenmayerTree : public GameObject {
public:
	// requestedSeed is the console's `seed <n>` (0 = none). The seed used is params.seed when that is non-zero,
	// else requestedSeed, else a fresh random one (see treeadapter::chooseSeed); it is logged and kept in getSeed().
	LindenmayerTree(const treegen::TreeParams &params, const Material &material = Material::bark5(), const Material &leavesMaterial = Material::leaves2(), bool useMeshWiring = false, bool normalVisualisation = false, uint32_t requestedSeed = 0);

	// Throws treegen::Error (RewriteError, InterpretError, ...) and std::length_error (mesh beyond the engine's int
	// limits); the object is left without a mesh then.
	void generate();

	bool meshWiring;
	bool normalVisualisation;

	treegen::TreeParams getTreeParams() { return params; }
	Material getBarkMaterial() { return barkMaterial; };
	Material getLeavesMaterial() { return leavesMaterial; }
	uint32_t getSeed() const { return seed; }

protected:
	Material barkMaterial;
	Material leavesMaterial;
	treegen::TreeParams params;

	std::shared_ptr<Mesh> mesh;

	int ringDensity = 9;

private:
	static Logger logger;

	uint32_t requestedSeed = 0;
	uint32_t seed = 0;

	void createMeshComponent(const treegen::MeshData &trunk);
	void createInstancedLeaves(const std::vector<glm::mat4> &leafTransforms);
	std::shared_ptr<InstancedMesh> createInstancedLeavesPanelMesh(std::vector<InstancedTransform> &instancedTransforms);
};

#endif
