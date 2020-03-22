#include "InstancedCube.h"

InstancedCube::InstancedCube(Material material, std::vector<InstancedTransform> &instancedTransforms) : GameObject() {
	std::shared_ptr<Shader> shader = ShaderManager::getShader("Shaders/InstancedShader.vert", "Shaders/InstancedShader.frag");
	addComponent(createMesh(shader, material, instancedTransforms));
}
void InstancedCube::addInstance(InstancedTransform transform) {
	mesh->addInstance(transform);
	mesh->updateMesh();
}

Vertex InstancedCube::createVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords) {
	Vertex vert;
	vert.position = position;
	vert.normal = normal;
	vert.texCoords = texCoords;

	return vert;
}

std::shared_ptr<Renderable> InstancedCube::createMesh(std::shared_ptr<Shader> shader, Material material, std::vector<InstancedTransform> &instancedTransforms) {
	std::vector<Vertex> vertices = {
		//front
		createVertex(glm::vec3(1.0f, -1.0f,  1.0f),		glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec2(1, 0)),
		createVertex(glm::vec3(1.0f,  1.0f,  1.0f),		glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec2(1, 1)),
		createVertex(glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec2(0, 1)),
		createVertex(glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec2(0, 0)),
		//back
		createVertex(glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(0, 0)),
		createVertex(glm::vec3(-1.0f,  1.0f, -1.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(0, 1)),
		createVertex(glm::vec3(1.0f,  1.0f, -1.0f),		glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(1, 1)),
		createVertex(glm::vec3(1.0f, -1.0f, -1.0f),		glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(1, 0)),
		//left
		createVertex(glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),	glm::vec2(0, 1)),
		createVertex(glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),	glm::vec2(1, 1)),
		createVertex(glm::vec3(-1.0f,  1.0f, -1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),	glm::vec2(1, 0)),
		createVertex(glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),	glm::vec2(0, 0)),
		//right
		createVertex(glm::vec3(1.0f, -1.0f, -1.0f),		glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec2(0, 0)),
		createVertex(glm::vec3(1.0f,  1.0f, -1.0f),		glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec2(1, 0)),
		createVertex(glm::vec3(1.0f,  1.0f,  1.0f),		glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec2(1, 1)),
		createVertex(glm::vec3(1.0f, -1.0f,  1.0f),		glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec2(0, 1)),
		//top
		createVertex(glm::vec3(1.0f, 1.0f,  1.0f),		glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(1, 1)),
		createVertex(glm::vec3(1.0f, 1.0f, -1.0f),		glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(1, 0)),
		createVertex(glm::vec3(-1.0f, 1.0f, -1.0f),		glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(0, 0)),
		createVertex(glm::vec3(-1.0f, 1.0f,  1.0f),		glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(0, 1)),
		//bottom
		createVertex(glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec3(0.0f, -1.0f, 0.0f),	glm::vec2(0, 1)),
		createVertex(glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec3(0.0f, -1.0f, 0.0f),	glm::vec2(0, 0)),
		createVertex(glm::vec3(1.0f, -1.0f, -1.0f),		glm::vec3(0.0f, -1.0f, 0.0f),	glm::vec2(1, 0)),
		createVertex(glm::vec3(1.0f, -1.0f,  1.0f),		glm::vec3(0.0f, -1.0f, 0.0f),	glm::vec2(1, 1)),
	};

	std::vector<int> indices = {
		0, 1, 3, 1, 2, 3,
		4, 5, 7, 5, 6, 7,
		8, 9, 11, 9, 10, 11,
		12, 13, 15, 13, 14, 15,
		16, 17, 19, 17, 18, 19,
		20, 21, 23, 21, 22, 23
	};

	mesh = std::shared_ptr<InstancedMesh>(new InstancedMesh(vertices, indices, instancedTransforms, shader, 100, 100, GL_STREAM_DRAW));

	mesh->material = material;
	return mesh;
}