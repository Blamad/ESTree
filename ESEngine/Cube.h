#pragma once
#include "Mesh.h"
#include "Vertex.h"
#include "Shader.h"
#include "GameObject.h"
#include "CubeBehaviour.h"
#include "Material.h"

using namespace glm;

class Cube : public GameObject {
public:
	//Default constr. for overriding
	Cube() : GameObject() { };

	Cube(bool any);
	Cube(vec4 color);
protected:
	Vertex createVertex(vec3 position, vec4 color, vec3 normal);
	shared_ptr<Mesh> createMesh(Shader shader);
	shared_ptr<Mesh> createMesh(Shader shader, vec4 color);
};