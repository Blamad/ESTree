#pragma once
#include "Engine/Manager/ShaderManager.h"
#include "Engine/GameObject/Mesh.h"
#include "Engine/Core/Vertex.h"
#include "GraphicEngine/Core/Shader.h"
#include "Engine/GameObject/GameObject.h"
#include "Engine/GameObject/Material.h"

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