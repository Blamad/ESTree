#include "Grass.h"

boost::variate_generator<boost::mt19937, boost::uniform_real<>> Grass::randomGenerator(boost::mt19937(time(0)), boost::uniform_real<>(-1, 1));

Grass::Grass(Material material) : GameObject() {
	shared_ptr<Shader> shader = ShaderManager::getShader("Shaders/GenericShader.vert", "Shaders/GenericShader.frag");

	float scale = 1;
	vector<InstancedTransform> transforms;
	for (int x = -10 * 2 * scale; x < 11 * 2 * scale; x+=2) {
		for (int z = -10 * 2 * scale; z < 11 * 2 * scale; z+=2) {
			InstancedTransform t;
			t.translateModel(vec3((x + randomGenerator()) * scale, 1*scale, (z + randomGenerator()) * scale));
			transforms.push_back(t);
		}
	}

	shared_ptr<InstancedMesh> mesh = createMesh(shader, transforms);
	mesh->material = material;
	addComponent(mesh);
};

Vertex Grass::createVertex(vec3 position, vec3 normal, vec2 texCoords) {
	Vertex vert;
	vert.position = position;
	vert.normal = normal;
	vert.texCoords = texCoords;

	return vert;
}

shared_ptr<InstancedMesh> Grass::createMesh(shared_ptr<Shader> shader, vector<InstancedTransform> transforms) {
	float scale = 0.5f;

	vector<Vertex> vertices = {

		//front
		createVertex(vec3(1.0f, -1.0f,  1.0f),		vec3(-0.5f, 0.0f, 0.5f),	vec2(1, 1)),
		createVertex(vec3(1.0f,  1.0f,  1.0f),		vec3(-0.5f, 0.0f, 0.5f),	vec2(1, 0)),
		createVertex(vec3(-1.0f,  1.0f,  -1.0f),	vec3(-0.5f, 0.0f, 0.5f),	vec2(0, 0)),
		createVertex(vec3(-1.0f, -1.0f,  -1.0f),	vec3(-0.5f, 0.0f, 0.5f),	vec2(0, 1)),
		//back
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.5f, 0.0f, -0.5f),	vec2(0, 1)),
		createVertex(vec3(-1.0f,  1.0f, -1.0f),	vec3(0.5f, 0.0f, -0.5f),	vec2(0, 0)),
		createVertex(vec3(1.0f,  1.0f, 1.0f),	vec3(0.5f, 0.0f, -0.5f),	vec2(1, 0)),
		createVertex(vec3(1.0f, -1.0f, 1.0f),	vec3(0.5f, 0.0f, -0.5f),	vec2(1, 1)),
		//left
		createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(-0.5f, 0.0f, -0.5f),	vec2(1, 1)),
		createVertex(vec3(-1.0f,  1.0f,  1.0f),	vec3(-0.5f, 0.0f, -0.5f),	vec2(1, 0)),
		createVertex(vec3(1.0f,  1.0f, -1.0f),	vec3(-0.5f, 0.0f, -0.5f),	vec2(0, 0)),
		createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(-0.5f, 0.0f, -0.5f),	vec2(0, 1)),
		//right
		createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(0.5f, 0.0f, 0.5f),		vec2(0, 1)),
		createVertex(vec3(1.0f,  1.0f, -1.0f),	vec3(0.5f, 0.0f, 0.5f),		vec2(0, 0)),
		createVertex(vec3(-1.0f,  1.0f,  1.0f),	vec3(0.5f, 0.0f, 0.5f),		vec2(1, 0)),
		createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(0.5f, 0.0f, 0.5f),		vec2(1, 1))
	};

	for (Vertex vert : vertices) {
		vert.position = vert.position * scale;
	}

	vector<int> indices = {
		0, 1, 3, 1, 2, 3,
		4, 5, 7, 5, 6, 7,
		8, 9, 11, 9, 10, 11,
		12, 13, 15, 13, 14, 15
	};

	shared_ptr<InstancedMesh> mesh = shared_ptr<InstancedMesh>(new InstancedMesh(vertices, indices, transforms, shader, vertices.size(), indices.size(), GL_STREAM_DRAW));
	return mesh;
}