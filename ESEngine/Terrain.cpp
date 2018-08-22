#include "Terrain.h"

Terrain::Terrain(Material material, bool useMeshWiring, bool useNormalVisualisation) : GameObject() {
	shared_ptr<Shader> shader = ShaderManager::getShader("Shaders/GenericShader.vert", "Shaders/GenericShader.frag");
	shared_ptr<Mesh> mesh = createMesh(shader);
	mesh->material = material;
	if (useMeshWiring)
		mesh->showMeshWiring();
	if (useNormalVisualisation)
		mesh->showNormalVisualisation();
	addComponent(mesh);
};

Vertex Terrain::createVertex(vec3 position, vec3 normal, vec2 texCoords) {
	Vertex vert;
	vert.position = position;
	vert.normal = normal;
	vert.texCoords = texCoords;

	return vert;
}

shared_ptr<Mesh> Terrain::createMesh(shared_ptr<Shader> shader) {
	vector<Vertex> v;
	vector<int> i;

	v.reserve(4);
	i.reserve(6);

	shared_ptr<Mesh> mesh = shared_ptr<Mesh>(new Mesh(v, i, shader, 100, 100, GL_STREAM_DRAW));

	vector<Vertex> vertices = {
		//top
		createVertex(vec3(1.0f, 0.0f,  1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(1, 1)),
		createVertex(vec3(1.0f, 0.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(1, 0)),
		createVertex(vec3(-1.0f, 0.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(0, 0)),
		createVertex(vec3(-1.0f, 0.0f,  1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(0, 1)),
	};

	vector<int> indices = {
		0, 1, 3, 1, 2, 3
	};

	for (int i = 0; i < vertices.size(); i++)
		mesh->vertices.push_back(vertices[i]);
	for (int i = 0; i < indices.size(); i++)
		mesh->indices.push_back(indices[i]);

	mesh->updateMesh();

	return mesh;
}