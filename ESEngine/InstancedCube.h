#pragma once
#include "InstancedMesh.h"
#include "Vertex.h"
#include "Shader.h"
#include "GameObject.h"
#include "Material.h"
#include "InstancedTransform.h"

using namespace glm;

class InstancedCube : public GameObject {
public:
	InstancedCube(Material material, vector<InstancedTransform> &instancedTransforms);

	void addInstance(InstancedTransform transform);

protected:
	shared_ptr<InstancedMesh> mesh;

	Vertex createVertex(vec3 position, vec3 normal, vec2 texCoords);
	shared_ptr<Renderable> createMesh(shared_ptr<Shader> shader, Material material, vector<InstancedTransform> &instancedTransforms);
};