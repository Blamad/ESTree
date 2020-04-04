#include "LindenmayerTree.h"
#include <glm/gtx/matrix_decompose.hpp>

boost::variate_generator<boost::mt19937, boost::uniform_real<> > LindenmayerTree::randomGenerator(boost::mt19937(time(0)), boost::uniform_real<>(0, 1));

Logger LindenmayerTree::logger("LindenmayerTree");

LindenmayerTree::LindenmayerTree(LindenmayerTreeParams &params, Material &material, Material &leavesMaterial, bool useMeshWiring, bool normalVisualisation) : params(params), barkMaterial(material), leavesMaterial(leavesMaterial), meshWiring(useMeshWiring), normalVisualisation(normalVisualisation), GameObject() {
	this->vBufferSize = 100000;
	this->iBufferSize = 4000000;
	this->ringDensity = 9;
}

void LindenmayerTree::generate() {
	createMeshComponent();
	generateTree();
}

void LindenmayerTree::generateTree() {
	logger.log(LOG_INFO, "Generation started: " + params.name);
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	generateMeshDescription();
	generateMeshData();
	generateInstancedLeaves();
	//generateLeaves();
	boost::posix_time::ptime endTime = boost::posix_time::microsec_clock::local_time();
	int totalMilisecs = (endTime - startTime).total_milliseconds();
	logger.log(LOG_INFO, "Generation finished. " + std::to_string(mesh->indices.size() / 3) + " tris. Time: " + std::to_string(totalMilisecs) + "ms");
}

void LindenmayerTree::generateTreeMesh() {
	generateMeshDescription();
	generateMeshData();
}

void LindenmayerTree::generateMeshData() {
	segmentsVec = LindenmayerTreeInterpreter::generateMeshData(params, product, meshGenerator.get());
	meshGenerator->generateMesh();
	mesh->updateMesh();
}

//PARSING L DATA
void LindenmayerTree::generateMeshDescription() {
	product = LindenmayerTreeSolver::generateTreeProduction(params);
}

//MESH AND VERTICES STUFF
void LindenmayerTree::createMeshComponent() {
	std::vector<Vertex> v;
	std::vector<int> i;

	v.reserve(vBufferSize);
	i.reserve(iBufferSize);

	std::shared_ptr<Shader> shader = ShaderManager::getShader("Resources/Shaders/GenericShader.vert", "Resources/Shaders/GenericShader.frag");
	mesh = std::shared_ptr<Mesh>(new Mesh(v, i, shader, vBufferSize, iBufferSize, GL_STREAM_DRAW));

	if (meshWiring)
		mesh->showMeshWiring();
	if (normalVisualisation)
		mesh->showNormalVisualisation();

	mesh->material = barkMaterial;
	addComponent(mesh);

	meshGenerator = std::make_unique<LindenmayerTreeMeshGenerator>(LindenmayerTreeMeshGenerator(mesh.get(), ringDensity));
}

// LEAVES

void LindenmayerTree::generateLeaves() {
	for (auto & seg : segmentsVec) {
		if (!seg->isLastStem())
			continue;
		if (randomGenerator() <= params.leafGrowthProb) {
			std::shared_ptr<GameObject> go(new GameObject());
			Transform* leafTransform = (Transform*)go->getComponent(TRANSFORM);

			if (params.leavesAngleDiversity > 0) { 
				glm::mat4 leafRotation(1);
				float angle = randomGenerator() * 2 * params.leavesAngleDiversity - params.leavesAngleDiversity;
				leafRotation = rotate(leafRotation, angle, glm::vec3(0,1,0));
				leafTransform->setModelMatrix(leafRotation * seg->modelMatrix);
			}
			else
				leafTransform->setModelMatrix(seg->modelMatrix);
			
			std::shared_ptr<Mesh> leafMesh = generateLeaf();
			go->addComponent(leafMesh);
			this->addGameObject(go);
		}
	}
}

void LindenmayerTree::generateInstancedLeaves() {
	std::vector<InstancedTransform> instancedTransforms;
	Transform *transform = (Transform*)getComponent(TRANSFORM);
	
	if (params.leafGrowthProb <= 0.0)
		return;

	for (auto & seg : segmentsVec) {
		if (!seg->isLastStem())
			continue;
		if (randomGenerator() <= params.leafGrowthProb) {
			InstancedTransform it;

			if (params.leavesAngleDiversity > 0) {
				glm::mat4 leafRotation(1);
				float angle = randomGenerator() * 2 * params.leavesAngleDiversity - params.leavesAngleDiversity;
				leafRotation = rotate(leafRotation, angle, glm::vec3(0, 1, 0));
				it.modelMatrix = transform->getModelMatrix() * seg->modelMatrix * leafRotation;
			}
			else
				it.modelMatrix = transform->getModelMatrix() * seg->modelMatrix;
			
			it.generateNormalModelMatrix();
			instancedTransforms.push_back(it);
		}
	}

	std::shared_ptr<InstancedMesh> leavesMesh = createInstancedLeavesPanelMesh(instancedTransforms);
	leavesMesh->initialTransfom = transform->getModelMatrix();
	std::shared_ptr<GameObject> go(new GameObject());
	go->addComponent(leavesMesh);
	this->addGameObject(go);
}

std::shared_ptr<Mesh> LindenmayerTree::generateLeaf() {
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

	std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(new Mesh(vertices, indices, shader, vertices.size(), indices.size(), GL_STREAM_DRAW));
	mesh->material = leavesMaterial;
	return mesh;
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

	std::shared_ptr<InstancedMesh> mesh = std::shared_ptr<InstancedMesh>(new InstancedMesh(vertices, indices, instancedTransforms, shader, vertices.size(), indices.size(), GL_STREAM_DRAW));
	mesh->material = leavesMaterial;
	return mesh;
}