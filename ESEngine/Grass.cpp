#include "Grass.h"

boost::variate_generator<boost::mt19937, boost::uniform_real<>> Grass::randomGenerator(boost::mt19937(time(0)), boost::uniform_real<>(-1, 1));

Grass::Grass(Material material) : GameObject() {
	std::shared_ptr<Shader> shader = ShaderManager::getShader("Shaders/GenericShader.vert", "Shaders/GenericShader.frag");

	float scale = 1;
	std::vector<InstancedTransform> transforms;
	for (int x = -10 * 2 * scale; x < 11 * 2 * scale; x+=2) {
		for (int z = -10 * 2 * scale; z < 11 * 2 * scale; z+=2) {
			InstancedTransform t;
			t.translateModel(glm::vec3((x + randomGenerator()) * scale, 1*scale, (z + randomGenerator()) * scale));
			transforms.push_back(t);
		}
	}

	std::shared_ptr<InstancedMesh> mesh = createMesh(shader, transforms);
	mesh->material = material;
	addComponent(mesh);
};

Vertex Grass::createVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords) {
	Vertex vert;
	vert.position = position;
	vert.normal = normal;
	vert.texCoords = texCoords;

	return vert;
}

std::shared_ptr<InstancedMesh> Grass::createMesh(std::shared_ptr<Shader> shader, std::vector<InstancedTransform> transforms) {
	float scale = 0.5f;

	std::vector<Vertex> vertices = {

		//front
		createVertex(glm::vec3(1.0f, -1.0f,  1.0f),		glm::vec3(-0.5f, 0.0f, 0.5f),	glm::vec2(1, 1)),
		createVertex(glm::vec3(1.0f,  1.0f,  1.0f),		glm::vec3(-0.5f, 0.0f, 0.5f),	glm::vec2(1, 0)),
		createVertex(glm::vec3(-1.0f,  1.0f,  -1.0f),	glm::vec3(-0.5f, 0.0f, 0.5f),	glm::vec2(0, 0)),
		createVertex(glm::vec3(-1.0f, -1.0f,  -1.0f),	glm::vec3(-0.5f, 0.0f, 0.5f),	glm::vec2(0, 1)),
		//back
		createVertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.5f, 0.0f, -0.5f),	glm::vec2(0, 1)),
		createVertex(glm::vec3(-1.0f,  1.0f, -1.0f),	glm::vec3(0.5f, 0.0f, -0.5f),	glm::vec2(0, 0)),
		createVertex(glm::vec3(1.0f,  1.0f, 1.0f),	glm::vec3(0.5f, 0.0f, -0.5f),	glm::vec2(1, 0)),
		createVertex(glm::vec3(1.0f, -1.0f, 1.0f),	glm::vec3(0.5f, 0.0f, -0.5f),	glm::vec2(1, 1)),
		//left
		createVertex(glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec3(-0.5f, 0.0f, -0.5f),	glm::vec2(1, 1)),
		createVertex(glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec3(-0.5f, 0.0f, -0.5f),	glm::vec2(1, 0)),
		createVertex(glm::vec3(1.0f,  1.0f, -1.0f),	glm::vec3(-0.5f, 0.0f, -0.5f),	glm::vec2(0, 0)),
		createVertex(glm::vec3(1.0f, -1.0f, -1.0f),	glm::vec3(-0.5f, 0.0f, -0.5f),	glm::vec2(0, 1)),
		//right
		createVertex(glm::vec3(1.0f, -1.0f, -1.0f),	glm::vec3(0.5f, 0.0f, 0.5f),		glm::vec2(0, 1)),
		createVertex(glm::vec3(1.0f,  1.0f, -1.0f),	glm::vec3(0.5f, 0.0f, 0.5f),		glm::vec2(0, 0)),
		createVertex(glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec3(0.5f, 0.0f, 0.5f),		glm::vec2(1, 0)),
		createVertex(glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec3(0.5f, 0.0f, 0.5f),		glm::vec2(1, 1))
	};

	for (Vertex vert : vertices) {
		vert.position = vert.position * scale;
	}

	std::vector<int> indices = {
		0, 1, 3, 1, 2, 3,
		4, 5, 7, 5, 6, 7,
		8, 9, 11, 9, 10, 11,
		12, 13, 15, 13, 14, 15
	};

	std::shared_ptr<InstancedMesh> mesh = std::shared_ptr<InstancedMesh>(new InstancedMesh(vertices, indices, transforms, shader, vertices.size(), indices.size(), GL_STREAM_DRAW));
	return mesh;
}