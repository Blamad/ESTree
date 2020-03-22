#pragma once
#include "ShaderManager.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Shader.h"
#include "GameObject.h"
#include "Material.h"

class Cube : public GameObject {
public:
	//Default constr. for overriding
	Cube() : GameObject() { };
	Cube(Material material, bool useMeshWiring = false, bool useNormalVisualisation = false);

protected:
	Vertex createVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords);
	Vertex createVertex(glm::vec3 position, glm::vec3 normal, glm::vec4 color);
	Vertex createVertex(glm::vec3 position, glm::vec3 normal);
	std::shared_ptr<Mesh> createMesh(std::shared_ptr<Shader> shader);
	std::shared_ptr<Mesh> createMesh(std::shared_ptr<Shader> shader, glm::vec4 color);
};