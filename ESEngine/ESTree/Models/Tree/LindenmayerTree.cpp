#include "LindenmayerTree.h"

#include <chrono>

#include "TreeMeshConversion.h"

Logger LindenmayerTree::logger("LindenmayerTree");

LindenmayerTree::LindenmayerTree(const treegen::TreeParams &params, const Material &material, const Material &leavesMaterial, bool useMeshWiring, bool normalVisualisation, uint32_t requestedSeed) : GameObject(), meshWiring(useMeshWiring), normalVisualisation(normalVisualisation), barkMaterial(material), leavesMaterial(leavesMaterial), params(params), requestedSeed(requestedSeed) {
}

void LindenmayerTree::generate() {
	if (mesh != nullptr) {
		logger.log(LOG_WARN, "Tree " + params.name + " is already generated");
		return;
	}

	seed = treeadapter::chooseSeed(params.seed, requestedSeed);
	logger.log(LOG_INFO, "Generation started: " + params.name + ". Seed: " + std::to_string(seed));
	const std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

	treegen::GenerateOptions options;
	options.seed = seed;
	options.ringDensity = ringDensity;
	const treegen::TreeModel model = treegen::generateTree(params, options);

	createMeshComponent(model.geometry.trunk);
	createInstancedLeaves(model.geometry.leafTransforms);

	const long long totalMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
	logger.log(LOG_INFO, "Generation finished. " + std::to_string(mesh->indices.size() / 3) + " tris. Time: " + std::to_string(totalMilliseconds) + "ms. Seed: " + std::to_string(seed));
}

void LindenmayerTree::createMeshComponent(const treegen::MeshData &trunk) {
	// Convert first: an index beyond the engine's int range throws before any GL object exists.
	std::vector<Vertex> vertices = treeadapter::toEngineVertices(trunk.vertices);
	std::vector<int> indices = treeadapter::toEngineIndices(trunk.indices);
	const int vBufferSize = treeadapter::checkedBufferSize(vertices.size(), "vertex");
	const int iBufferSize = treeadapter::checkedBufferSize(indices.size(), "index");

	std::shared_ptr<Shader> shader = ShaderManager::getShader("Resources/Shaders/GenericShader.vert", "Resources/Shaders/GenericShader.frag");

	// The buffers are created with exactly the generated sizes; the CPU vectors are moved in afterwards and
	// uploaded once (a tree is generated once per object, GL_STREAM_DRAW is kept from before).
	std::vector<Vertex> noVertices;
	std::vector<int> noIndices;
	std::shared_ptr<Mesh> trunkMesh = std::make_shared<Mesh>(noVertices, noIndices, shader, vBufferSize, iBufferSize, GL_STREAM_DRAW);
	trunkMesh->vertices = std::move(vertices);
	trunkMesh->indices = std::move(indices);
	if (!trunkMesh->vertices.empty())
		trunkMesh->updateMesh();

	if (meshWiring)
		trunkMesh->showMeshWiring();
	if (normalVisualisation)
		trunkMesh->showNormalVisualisation();

	trunkMesh->material = barkMaterial;
	mesh = trunkMesh;
	addComponent(mesh);
}

void LindenmayerTree::createInstancedLeaves(const std::vector<glm::mat4> &leafTransforms) {
	// No leaves (leafGrowthProb <= 0 or nothing kept): no mesh, an instanced buffer cannot be created empty.
	if (leafTransforms.empty())
		return;

	Transform *transform = getComponentAs<Transform>(TRANSFORM);
	std::vector<InstancedTransform> instancedTransforms = treeadapter::toInstancedTransforms(leafTransforms, transform->getModelMatrix());

	std::shared_ptr<InstancedMesh> leavesMesh = createInstancedLeavesPanelMesh(instancedTransforms);
	leavesMesh->initialTransfom = transform->getModelMatrix();
	std::shared_ptr<GameObject> go(new GameObject());
	go->addComponent(leavesMesh);
	this->addGameObject(go);
}

std::shared_ptr<InstancedMesh> LindenmayerTree::createInstancedLeavesPanelMesh(std::vector<InstancedTransform> &instancedTransforms) {
	std::shared_ptr<Shader> shader = ShaderManager::getShader("Resources/Shaders/GenericShader.vert", "Resources/Shaders/GenericShader.frag");

	std::vector<Vertex> vertices = {
		//front
		Vertex::createVertex(glm::vec3(1.0f, -1.0f,  0.0f),	glm::vec3(0.0f, 0.0f, 1.0f),		glm::vec2(1, 1)),
		Vertex::createVertex(glm::vec3(1.0f,  1.0f,  0.0f),	glm::vec3(0.0f, 0.0f, 1.0f),		glm::vec2(1, 0)),
		Vertex::createVertex(glm::vec3(-1.0f,  1.0f,  0.0f),	glm::vec3(0.0f, 0.0f, 1.0f),		glm::vec2(0, 0)),
		Vertex::createVertex(glm::vec3(-1.0f, -1.0f,  0.0f),	glm::vec3(0.0f, 0.0f, 1.0f),		glm::vec2(0, 1)),
		//back
		Vertex::createVertex(glm::vec3(-1.0f, -1.0f, 0.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(0, 1)),
		Vertex::createVertex(glm::vec3(-1.0f,  1.0f, 0.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(0, 0)),
		Vertex::createVertex(glm::vec3(1.0f,  1.0f, 0.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(1, 0)),
		Vertex::createVertex(glm::vec3(1.0f, -1.0f, 0.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(1, 1)),
	};

	std::vector<int> indices = {
		0, 1, 3, 1, 2, 3,
		4, 5, 7, 5, 6, 7,
	};

	std::shared_ptr<InstancedMesh> leavesMesh = std::shared_ptr<InstancedMesh>(new InstancedMesh(vertices, indices, instancedTransforms, shader, static_cast<int>(vertices.size()), static_cast<int>(indices.size()), GL_STREAM_DRAW));
	leavesMesh->material = leavesMaterial;
	return leavesMesh;
}
