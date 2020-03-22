#ifndef TERRAIN_H
#define TERRAIN_H

#include "ShaderManager.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Shader.h"
#include "GameObject.h"
#include "Material.h"

class Terrain : public GameObject {
public:
	Terrain() : GameObject() { };
	Terrain(Material material, bool useMeshWiring = false, bool useNormalVisualisation = false);

protected:
	Vertex createVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords);
	std::shared_ptr<Mesh> createMesh(std::shared_ptr<Shader> shader);
};

#endif