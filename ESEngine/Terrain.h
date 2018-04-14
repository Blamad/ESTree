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
	Vertex createVertex(vec3 position, vec3 normal, vec2 texCoords);
	shared_ptr<Mesh> createMesh(shared_ptr<Shader> shader);
};

#endif