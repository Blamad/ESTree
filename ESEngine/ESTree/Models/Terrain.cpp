#include "Terrain.h"

Terrain::Terrain(Material material, bool useMeshWiring, bool useNormalVisualisation) : GameObject() {
	std::shared_ptr<Shader> shader = ShaderManager::getShader("Resources/Shaders/GenericShader.vert", "Resources/Shaders/GenericShader.frag");
	std::shared_ptr<Mesh> mesh = createMesh(shader);
	mesh->material = material;
	if (useMeshWiring)
		mesh->showMeshWiring();
	if (useNormalVisualisation)
		mesh->showNormalVisualisation();
	addComponent(mesh);
};

Vertex Terrain::createVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords) {
	Vertex vert;
	vert.position = position;
	vert.normal = normal;
	vert.texCoords = texCoords;

	return vert;
}

std::shared_ptr<Mesh> Terrain::createMesh(std::shared_ptr<Shader> shader) {
	std::vector<Vertex> v;
	std::vector<int> i;

	v.reserve(4);
	i.reserve(6);

	std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(new Mesh(v, i, shader, 100, 100, GL_STREAM_DRAW));

	std::vector<Vertex> vertices = {
		//top
		createVertex(glm::vec3(1.0f, 0.0f,  1.0f),	glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(1, 1)),
		createVertex(glm::vec3(1.0f, 0.0f, -1.0f),	glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(1, 0)),
		createVertex(glm::vec3(-1.0f, 0.0f, -1.0f),	glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(0, 0)),
		createVertex(glm::vec3(-1.0f, 0.0f,  1.0f),	glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(0, 1)),
	};

	std::vector<int> indices = {
		0, 1, 3, 
		1, 2, 3
	};

	for (int i = 0; i < vertices.size(); i++)
		mesh->vertices.push_back(vertices[i]);
	for (int i = 0; i < indices.size(); i++)
		mesh->indices.push_back(indices[i]);

	mesh->updateMesh();

	return mesh;
}