#include "Cube.h"

Cube::Cube(Material material, bool useMeshWiring, bool useNormalVisualisation) : GameObject() {
	std::shared_ptr<Shader> shader = ShaderManager::getShader("Shaders/GenericShader.vert", "Shaders/GenericShader.frag");
	std::shared_ptr<Mesh> mesh = createMesh(shader);
	mesh->material = material;
	if (useMeshWiring)
		mesh->showMeshWiring();
	if (useNormalVisualisation)
		mesh->showNormalVisualisation();
	addComponent(mesh);
};

Vertex Cube::createVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords) {
	Vertex vert;
	vert.position = position;
	vert.normal = normal;
	vert.texCoords = texCoords;

	return vert;
}

Vertex Cube::createVertex(glm::vec3 position, glm::vec3 normal, glm::vec4 color) {
	Vertex vert;
	vert.position = position;
	vert.color = color;
	vert.normal = normal;

	return vert;
}

Vertex Cube::createVertex(glm::vec3 position, glm::vec3 normal) {
	Vertex vert;
	vert.position = position;
	vert.normal = normal;

	return vert;
}

std::shared_ptr<Mesh> Cube::createMesh(std::shared_ptr<Shader> shader) {
	std::vector<Vertex> v;
	std::vector<int> i;

	v.reserve(100);
	i.reserve(100);

	std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(new Mesh(v, i, shader, 100, 100, GL_STREAM_DRAW));

	std::vector<Vertex> vertices = {
		//front
		createVertex(glm::vec3( 1.0f, -1.0f,  1.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec2(1, 0)),
		createVertex(glm::vec3( 1.0f,  1.0f,  1.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec2(1, 1)),
		createVertex(glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec2(0, 1)),
		createVertex(glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec2(0, 0)),
		//back
		createVertex(glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(0, 0)),
		createVertex(glm::vec3(-1.0f,  1.0f, -1.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(0, 1)),
		createVertex(glm::vec3( 1.0f,  1.0f, -1.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(1, 1)),
		createVertex(glm::vec3( 1.0f, -1.0f, -1.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(1, 0)),
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
		createVertex(glm::vec3( 1.0f, 1.0f,  1.0f),		glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(1, 1)),
		createVertex(glm::vec3( 1.0f, 1.0f, -1.0f),		glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(1, 0)),
		createVertex(glm::vec3(-1.0f, 1.0f, -1.0f),		glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(0, 0)),
		createVertex(glm::vec3(-1.0f, 1.0f,  1.0f),		glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(0, 1)),
		//bottom
		createVertex(glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec3(0.0f, -1.0f, 0.0f),	glm::vec2(0, 1)),
		createVertex(glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec3(0.0f, -1.0f, 0.0f),	glm::vec2(0, 0)),
		createVertex(glm::vec3( 1.0f, -1.0f, -1.0f),	glm::vec3(0.0f, -1.0f, 0.0f),	glm::vec2(1, 0)),
		createVertex(glm::vec3( 1.0f, -1.0f,  1.0f),	glm::vec3(0.0f, -1.0f, 0.0f),	glm::vec2(1, 1)),
	};

	std::vector<int> indices = {
		0, 1, 3, 1, 2, 3,
		4, 5, 7, 5, 6, 7,
		8, 9, 11, 9, 10, 11,
		12, 13, 15, 13, 14, 15,
		16, 17, 19, 17, 18, 19,
		20, 21, 23, 21, 22, 23
	};

	for (int i = 0; i < vertices.size(); i++)
		mesh->vertices.push_back(vertices[i]);
	for (int i = 0; i < indices.size(); i++)
		mesh->indices.push_back(indices[i]);

	mesh->updateMesh();
//	std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(new Mesh(vertices, indices, shader));

	return mesh;
}

std::shared_ptr<Mesh> Cube::createMesh(std::shared_ptr<Shader> shader, glm::vec4 color) {
	std::vector<Vertex> vertices = {
		//front
		createVertex(glm::vec3(1.0f, -1.0f, 1.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	color),
		createVertex(glm::vec3(1.0f, 1.0f, 1.0f),	glm::vec3(0.0f, 0.0f, 1.0f), color),
		createVertex(glm::vec3(-1.0f, 1.0f, 1.0f),	glm::vec3(0.0f, 0.0f, 1.0f), color),
		createVertex(glm::vec3(-1.0f, -1.0f, 1.0f),	glm::vec3(0.0f, 0.0f, 1.0f), color),
		//back
		createVertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), color),
		createVertex(glm::vec3(-1.0f, 1.0f, -1.0f),	glm::vec3(0.0f, 0.0f, -1.0f), color),
		createVertex(glm::vec3(1.0f, 1.0f, -1.0f),	glm::vec3(0.0f, 0.0f, -1.0f), color),
		createVertex(glm::vec3(1.0f, -1.0f, -1.0f),	glm::vec3(0.0f, 0.0f, -1.0f), color),
		//left
		createVertex(glm::vec3(-1.0f, -1.0f, 1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f), color),
		createVertex(glm::vec3(-1.0f, 1.0f, 1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f), color),
		createVertex(glm::vec3(-1.0f, 1.0f, -1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f), color),
		createVertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), color),
		//right
		createVertex(glm::vec3(1.0f, -1.0f, -1.0f),	glm::vec3(1.0f, 0.0f, 0.0f), color),
		createVertex(glm::vec3(1.0f, 1.0f, -1.0f),	glm::vec3(1.0f, 0.0f, 0.0f), color),
		createVertex(glm::vec3(1.0f, 1.0f, 1.0f),	glm::vec3(1.0f, 0.0f, 0.0f), color),
		createVertex(glm::vec3(1.0f, -1.0f, 1.0f),	glm::vec3(1.0f, 0.0f, 0.0f), color),
		//top
		createVertex(glm::vec3(1.0f, 1.0f, 1.0f),	glm::vec3(0.0f, 1.0f, 0.0f), color),
		createVertex(glm::vec3(1.0f, 1.0f, -1.0f),	glm::vec3(0.0f, 1.0f, 0.0f), color),
		createVertex(glm::vec3(-1.0f, 1.0f, -1.0f),	glm::vec3(0.0f, 1.0f, 0.0f), color),
		createVertex(glm::vec3(-1.0f, 1.0f, 1.0f),	glm::vec3(0.0f, 1.0f, 0.0f), color),
		//bottom
		createVertex(glm::vec3(-1.0f, -1.0f, 1.0f),	glm::vec3(0.0f, -1.0f, 0.0f), color),
		createVertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f), color),
		createVertex(glm::vec3(1.0f, -1.0f, -1.0f),	glm::vec3(0.0f, -1.0f, 0.0f), color),
		createVertex(glm::vec3(1.0f, -1.0f, 1.0f),	glm::vec3(0.0f, -1.0f, 0.0f), color),
	};

	std::vector<int> indices = {
		0, 1, 3, 1, 2, 3,
		4, 5, 7, 5, 6, 7,
		8, 9, 11, 9, 10, 11,
		12, 13, 15, 13, 14, 15,
		16, 17, 19, 17, 18, 19,
		20, 21, 23, 21, 22, 23
	};

	return std::shared_ptr<Mesh>(new Mesh(vertices, indices, shader));
}