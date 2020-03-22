#pragma once
#include "InstancedMesh.h"
#include "Vertex.h"
#include "Shader.h"
#include "GameObject.h"
#include "Material.h"
#include "InstancedTransform.h"

class InstancedCube : public GameObject {
public:
	InstancedCube(Material material, std::vector<InstancedTransform> &instancedTransforms);

	void addInstance(InstancedTransform transform);

protected:
	std::shared_ptr<InstancedMesh> mesh;

	Vertex createVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords);
	std::shared_ptr<Renderable> createMesh(std::shared_ptr<Shader> shader, Material material, std::vector<InstancedTransform> &instancedTransforms);
};