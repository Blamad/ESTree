#include "Cube.h"

Cube::Cube(Material material, bool useMeshWiring, bool useNormalVisualisation) : GameObject() {
	shared_ptr<Shader> shader = ShaderManager::getInstance().getShader("Shaders/GenericShader.vert", "Shaders/GenericShader.frag");
	shared_ptr<Mesh> mesh = createMesh(shader);
	mesh->material = material;
	if (useMeshWiring)
		mesh->showMeshWiring();
	if (useNormalVisualisation)
		mesh->showNormalVisualisation();
	addComponent(mesh);
};

Vertex Cube::createVertex(vec3 position, vec3 normal, vec2 texCoords) {
	Vertex vert;
	vert.position = position;
	vert.normal = normal;
	vert.texCoords = texCoords;

	return vert;
}

Vertex Cube::createVertex(vec3 position, vec3 normal, vec4 color) {
	Vertex vert;
	vert.position = position;
	vert.color = color;
	vert.normal = normal;

	return vert;
}

Vertex Cube::createVertex(vec3 position, vec3 normal) {
	Vertex vert;
	vert.position = position;
	vert.normal = normal;

	return vert;
}

shared_ptr<Mesh> Cube::createMesh(shared_ptr<Shader> shader) {
	vector<Vertex> v;
	vector<int> i;

	v.reserve(100);
	i.reserve(100);

	shared_ptr<Mesh> mesh = shared_ptr<Mesh>(new Mesh(v, i, shader, 100, 100, GL_STREAM_DRAW));

	vector<Vertex> vertices = {
		//front
		createVertex(vec3( 1.0f, -1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 0)),
		createVertex(vec3( 1.0f,  1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 1)),
		createVertex(vec3(-1.0f,  1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 1)),
		createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 0)),
		//back
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),	vec2(0, 0)),
		createVertex(vec3(-1.0f,  1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(0, 1)),
		createVertex(vec3( 1.0f,  1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 1)),
		createVertex(vec3( 1.0f, -1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 0)),
		//left
		createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(-1.0f, 0.0f, 0.0f),	vec2(0, 1)),
		createVertex(vec3(-1.0f,  1.0f,  1.0f),	vec3(-1.0f, 0.0f, 0.0f),	vec2(1, 1)),
		createVertex(vec3(-1.0f,  1.0f, -1.0f),	vec3(-1.0f, 0.0f, 0.0f),	vec2(1, 0)),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(-1.0f, 0.0f, 0.0f),	vec2(0, 0)),
		//right
		createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(0, 0)),
		createVertex(vec3(1.0f,  1.0f, -1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(1, 0)),
		createVertex(vec3(1.0f,  1.0f,  1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(1, 1)),
		createVertex(vec3(1.0f, -1.0f,  1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(0, 1)),
		//top
		createVertex(vec3( 1.0f, 1.0f,  1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(1, 1)),
		createVertex(vec3( 1.0f, 1.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(1, 0)),
		createVertex(vec3(-1.0f, 1.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(0, 0)),
		createVertex(vec3(-1.0f, 1.0f,  1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(0, 1)),
		//bottom
		createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(0.0f, -1.0f, 0.0f),	vec2(0, 1)),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f),	vec2(0, 0)),
		createVertex(vec3( 1.0f, -1.0f, -1.0f),	vec3(0.0f, -1.0f, 0.0f),	vec2(1, 0)),
		createVertex(vec3( 1.0f, -1.0f,  1.0f),	vec3(0.0f, -1.0f, 0.0f),	vec2(1, 1)),
	};

	vector<int> indices = {
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

	//shared_ptr<Mesh> mesh = shared_ptr<Mesh>(new Mesh(vertices, indices, shader));

	return mesh;
}

shared_ptr<Mesh> Cube::createMesh(shared_ptr<Shader> shader, vec4 color) {
	vector<Vertex> vertices = {
		//front
		createVertex(vec3(1.0f, -1.0f, 1.0f),	vec3(0.0f, 0.0f, 1.0f),	color),
		createVertex(vec3(1.0f, 1.0f, 1.0f),	vec3(0.0f, 0.0f, 1.0f), color),
		createVertex(vec3(-1.0f, 1.0f, 1.0f),	vec3(0.0f, 0.0f, 1.0f), color),
		createVertex(vec3(-1.0f, -1.0f, 1.0f),	vec3(0.0f, 0.0f, 1.0f), color),
		//back
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f), color),
		createVertex(vec3(-1.0f, 1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f), color),
		createVertex(vec3(1.0f, 1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f), color),
		createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f), color),
		//left
		createVertex(vec3(-1.0f, -1.0f, 1.0f),	vec3(-1.0f, 0.0f, 0.0f), color),
		createVertex(vec3(-1.0f, 1.0f, 1.0f),	vec3(-1.0f, 0.0f, 0.0f), color),
		createVertex(vec3(-1.0f, 1.0f, -1.0f),	vec3(-1.0f, 0.0f, 0.0f), color),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(-1.0f, 0.0f, 0.0f), color),
		//right
		createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(1.0f, 0.0f, 0.0f), color),
		createVertex(vec3(1.0f, 1.0f, -1.0f),	vec3(1.0f, 0.0f, 0.0f), color),
		createVertex(vec3(1.0f, 1.0f, 1.0f),	vec3(1.0f, 0.0f, 0.0f), color),
		createVertex(vec3(1.0f, -1.0f, 1.0f),	vec3(1.0f, 0.0f, 0.0f), color),
		//top
		createVertex(vec3(1.0f, 1.0f, 1.0f),	vec3(0.0f, 1.0f, 0.0f), color),
		createVertex(vec3(1.0f, 1.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f), color),
		createVertex(vec3(-1.0f, 1.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f), color),
		createVertex(vec3(-1.0f, 1.0f, 1.0f),	vec3(0.0f, 1.0f, 0.0f), color),
		//bottom
		createVertex(vec3(-1.0f, -1.0f, 1.0f),	vec3(0.0f, -1.0f, 0.0f), color),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f), color),
		createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(0.0f, -1.0f, 0.0f), color),
		createVertex(vec3(1.0f, -1.0f, 1.0f),	vec3(0.0f, -1.0f, 0.0f), color),
	};

	vector<int> indices = {
		0, 1, 3, 1, 2, 3,
		4, 5, 7, 5, 6, 7,
		8, 9, 11, 9, 10, 11,
		12, 13, 15, 13, 14, 15,
		16, 17, 19, 17, 18, 19,
		20, 21, 23, 21, 22, 23
	};

	return shared_ptr<Mesh>(new Mesh(vertices, indices, shader));
}