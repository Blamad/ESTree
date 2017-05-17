#include "Cube.h"

Cube::Cube(vec4 color) : GameObject() {
	Shader shader("GenericShader.vert", "GenericShader.frag");
	shared_ptr<Mesh> mesh = createMesh(shader, color);
	mesh->material = Material::ruby();
	shared_ptr<Transform> transform = shared_ptr<Transform>(new Transform());
	shared_ptr<Behaviour> behaviour = shared_ptr<Behaviour>(new CubeBehaviour());

	addComponent(mesh);
	addComponent(transform);
	addComponent(behaviour);
}

Cube::Cube(bool any) : GameObject() {
	Shader shader("GenericShader.vert", "GenericShader.frag");
	shared_ptr<Mesh> mesh = createMesh(shader);
	shared_ptr<Transform> transform = shared_ptr<Transform>(new Transform());
	shared_ptr<Behaviour> behaviour = shared_ptr<Behaviour>(new CubeBehaviour());

	addComponent(mesh);
	addComponent(transform);
	addComponent(behaviour);
};

Vertex Cube::createVertex(vec3 position, vec3 normal, vec4 color) {
	Vertex vert;
	vert.position = position;
	vert.color = color;
	vert.normal = normal;
	vert.type[POSITION] = 1;
	vert.type[COLOR] = 1;
	vert.type[NORMAL] = 1;

	return vert;
}

Vertex Cube::createVertex(vec3 position, vec3 normal) {
	Vertex vert;
	vert.position = position;
	vert.normal = normal;
	vert.type[POSITION] = 1;
	vert.type[NORMAL] = 1;

	return vert;
}

shared_ptr<Mesh> Cube::createMesh(Shader shader) {
	vector<Vertex> verticies = {
		//front
		createVertex(vec3(1.0f, -1.0f, 1.0f),	vec3(0.0f, 0.0f, 1.0f)),
		createVertex(vec3(1.0f, 1.0f, 1.0f),	vec3(0.0f, 0.0f, 1.0f)),
		createVertex(vec3(-1.0f, 1.0f, 1.0f),	vec3(0.0f, 0.0f, 1.0f)),
		createVertex(vec3(-1.0f, -1.0f, 1.0f),	vec3(0.0f, 0.0f, 1.0f)),
		//back
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f)),
		createVertex(vec3(-1.0f, 1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f)),
		createVertex(vec3(1.0f, 1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f)),
		createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f)),
		//left
		createVertex(vec3(-1.0f, -1.0f, 1.0f),	vec3(-1.0f, 0.0f, 0.0f)),
		createVertex(vec3(-1.0f, 1.0f, 1.0f),	vec3(-1.0f, 0.0f, 0.0f)),
		createVertex(vec3(-1.0f, 1.0f, -1.0f),	vec3(-1.0f, 0.0f, 0.0f)),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(-1.0f, 0.0f, 0.0f)),
		//right
		createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(1.0f, 0.0f, 0.0f)),
		createVertex(vec3(1.0f, 1.0f, -1.0f),	vec3(1.0f, 0.0f, 0.0f)),
		createVertex(vec3(1.0f, 1.0f, 1.0f),	vec3(1.0f, 0.0f, 0.0f)),
		createVertex(vec3(1.0f, -1.0f, 1.0f),	vec3(1.0f, 0.0f, 0.0f)),
		//top
		createVertex(vec3(1.0f, 1.0f, 1.0f),	vec3(0.0f, 1.0f, 0.0f)),
		createVertex(vec3(1.0f, 1.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f)),
		createVertex(vec3(-1.0f, 1.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f)),
		createVertex(vec3(-1.0f, 1.0f, 1.0f),	vec3(0.0f, 1.0f, 0.0f)),
		//bottom
		createVertex(vec3(-1.0f, -1.0f, 1.0f),	vec3(0.0f, -1.0f, 0.0f)),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f)),
		createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(0.0f, -1.0f, 0.0f)),
		createVertex(vec3(1.0f, -1.0f, 1.0f),	vec3(0.0f, -1.0f, 0.0f)),
	};

	vector<int> indicies = {
		0, 1, 3, 1, 2, 3,
		4, 5, 7, 5, 6, 7,
		8, 9, 11, 9, 10, 11,
		12, 13, 15, 13, 14, 15,
		16, 17, 19, 17, 18, 19,
		20, 21, 23, 21, 22, 23
	};

	return shared_ptr<Mesh>(new Mesh(verticies, indicies, shader));
}

shared_ptr<Mesh> Cube::createMesh(Shader shader, vec4 color) {
	vector<Vertex> verticies = {
		//front
		createVertex(vec3(1.0f, -1.0f, 1.0f),	vec3(0.0f, 0.0f, 1.0f), color),
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

	vector<int> indicies = {
		0, 1, 3, 1, 2, 3,
		4, 5, 7, 5, 6, 7,
		8, 9, 11, 9, 10, 11,
		12, 13, 15, 13, 14, 15,
		16, 17, 19, 17, 18, 19,
		20, 21, 23, 21, 22, 23
	};

	return shared_ptr<Mesh>(new Mesh(verticies, indicies, shader));
}