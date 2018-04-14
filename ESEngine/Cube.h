#pragma once
#include "ShaderManager.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Shader.h"
#include "GameObject.h"
#include "Material.h"

using namespace glm;

class Cube : public GameObject {
public:
	//Default constr. for overriding
	Cube() : GameObject() { };
	Cube(Material material, bool useMeshWiring = false, bool useNormalVisualisation = false);

protected:
	Vertex createVertex(vec3 position, vec3 normal, vec2 texCoords);
	Vertex createVertex(vec3 position, vec3 normal, vec4 color);
	Vertex createVertex(vec3 position, vec3 normal);
	shared_ptr<Mesh> createMesh(shared_ptr<Shader> shader);
	shared_ptr<Mesh> createMesh(shared_ptr<Shader> shader, vec4 color);
};