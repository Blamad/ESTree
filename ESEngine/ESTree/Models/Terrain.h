#ifndef TERRAIN_H
#define TERRAIN_H

#include "Engine/Manager/ShaderManager.h"
#include "Engine/GameObject/Mesh.h"
#include "Engine/Core/Vertex.h"
#include "GraphicEngine/Core/Shader.h"
#include "Engine/GameObject/GameObject.h"
#include "Engine/GameObject/Material.h"

class Terrain : public GameObject {
public:
	Terrain() : GameObject() { };
	Terrain(Material material, bool useMeshWiring = false, bool useNormalVisualisation = false);

protected:
	Vertex createVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords);
	std::shared_ptr<Mesh> createMesh(std::shared_ptr<Shader> shader);
};

#endif