#include "Cube.h"

Cube::Cube(vec4 color) : GameObject() {
	Shader shader("GenericShader.vert", "GenericShader.frag");
	shared_ptr<Mesh> mesh = createMesh(shader, color);
	mesh->material = Material::cyanPlastic();
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

Vertex Cube::createVertex(vec3 position, vec4 color, vec3 normal) {
	Vertex vert;
	vert.position = position;
	vert.color = color;
	vert.normal = normal;
	vert.type[POSITION] = 1;
	vert.type[COLOR] = 1;
	vert.type[NORMAL] = 1;

	return vert;
}

shared_ptr<Mesh> Cube::createMesh(Shader shader) {
	vector<Vertex> verticies = {
		//front
		createVertex(vec3(1.0f, 1.0f, 1.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
		createVertex(vec3(1.0f, -1.0f, 1.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
		createVertex(vec3(-1.0f, -1.0f, 1.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
		createVertex(vec3(-1.0f, 1.0f, 1.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
		//back
		createVertex(vec3(1.0f, 1.0f, -1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)),
		createVertex(vec3(1.0f, -1.0f, -1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)),
		createVertex(vec3(-1.0f, 1.0f, -1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)),
		//left
		createVertex(vec3(-1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
		createVertex(vec3(-1.0f, 1.0f, -1.0f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
		createVertex(vec3(-1.0f, -1.0f, 1.0f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
		//right
		createVertex(vec3(1.0f, 1.0f, 1.0f), vec4(0.0f, 1.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),
		createVertex(vec3(1.0f, 1.0f, -1.0f), vec4(0.0f, 1.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),
		createVertex(vec3(1.0f, -1.0f, -1.0f), vec4(0.0f, 1.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),
		createVertex(vec3(1.0f, -1.0f, 1.0f), vec4(0.0f, 1.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),
		//top
		createVertex(vec3(1.0f, -1.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
		createVertex(vec3(1.0f, -1.0f, -1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
		createVertex(vec3(-1.0f, -1.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
		//bottom
		createVertex(vec3(1.0f, 1.0f, 1.0f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
		createVertex(vec3(1.0f, 1.0f, -1.0f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
		createVertex(vec3(-1.0f, 1.0f, -1.0f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
		createVertex(vec3(-1.0f, 1.0f, 1.0f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
	};

	vector<int> indicies = {
		0, 1, 3, 1, 2, 3,
		7, 5, 4, 7, 6, 5,
		8, 11, 9, 9, 11, 10,
		12, 15, 14, 12, 14, 13,
		16, 17, 19, 19, 17, 18,
		20, 23, 21, 23, 22, 21
	};

	return shared_ptr<Mesh>(new Mesh(verticies, indicies, shader));
}

shared_ptr<Mesh> Cube::createMesh(Shader shader, vec4 color) {
	vector<Vertex> verticies = {
		//front
		createVertex(vec3(1.0f, 1.0f, 1.0f), color, vec3(0.0f, 0.0f, 1.0f)),
		createVertex(vec3(1.0f, -1.0f, 1.0f), color, vec3(0.0f, 0.0f, 1.0f)),
		createVertex(vec3(-1.0f, -1.0f, 1.0f), color, vec3(0.0f, 0.0f, 1.0f)),
		createVertex(vec3(-1.0f, 1.0f, 1.0f), color, vec3(0.0f, 0.0f, 1.0f)),
		//back
		createVertex(vec3(1.0f, 1.0f, -1.0f), color, vec3(0.0f, 0.0f, -1.0f)),
		createVertex(vec3(1.0f, -1.0f, -1.0f), color, vec3(0.0f, 0.0f, -1.0f)),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), color, vec3(0.0f, 0.0f, -1.0f)),
		createVertex(vec3(-1.0f, 1.0f, -1.0f), color, vec3(0.0f, 0.0f, -1.0f)),
		//left
		createVertex(vec3(-1.0f, 1.0f, 1.0f), color, vec3(-1.0f, 0.0f, 0.0f)),
		createVertex(vec3(-1.0f, 1.0f, -1.0f), color, vec3(-1.0f, 0.0f, 0.0f)),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), color, vec3(-1.0f, 0.0f, 0.0f)),
		createVertex(vec3(-1.0f, -1.0f, 1.0f), color, vec3(-1.0f, 0.0f, 0.0f)),
		//right
		createVertex(vec3(1.0f, 1.0f, 1.0f), color, vec3(1.0f, 0.0f, 0.0f)),
		createVertex(vec3(1.0f, 1.0f, -1.0f), color, vec3(1.0f, 0.0f, 0.0f)),
		createVertex(vec3(1.0f, -1.0f, -1.0f), color, vec3(1.0f, 0.0f, 0.0f)),
		createVertex(vec3(1.0f, -1.0f, 1.0f), color, vec3(1.0f, 0.0f, 0.0f)),
		//top
		createVertex(vec3(1.0f, -1.0f, 1.0f), color, vec3(0.0f, -1.0f, 0.0f)),
		createVertex(vec3(1.0f, -1.0f, -1.0f), color, vec3(0.0f, -1.0f, 0.0f)),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), color, vec3(0.0f, -1.0f, 0.0f)),
		createVertex(vec3(-1.0f, -1.0f, 1.0f), color, vec3(0.0f, -1.0f, 0.0f)),
		//bottom
		createVertex(vec3(1.0f, 1.0f, 1.0f), color, vec3(0.0f, 1.0f, 0.0f)),
		createVertex(vec3(1.0f, 1.0f, -1.0f), color, vec3(0.0f, 1.0f, 0.0f)),
		createVertex(vec3(-1.0f, 1.0f, -1.0f), color, vec3(0.0f, 1.0f, 0.0f)),
		createVertex(vec3(-1.0f, 1.0f, 1.0f), color, vec3(0.0f, 1.0f, 0.0f)),
	};
	vector<int> indicies = {
		0, 1, 3, 1, 2, 3,
		7, 5, 4, 7, 6, 5,
		8, 11, 9, 9, 11, 10,
		12, 15, 14, 12, 14, 13,
		16, 17, 19, 19, 17, 18,
		20, 23, 21, 23, 22, 21
	};

	return shared_ptr<Mesh>(new Mesh(verticies, indicies, shader));
}