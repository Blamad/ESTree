#pragma once
#include "ShaderManager.h"
#include "InstancedMesh.h"
#include "Vertex.h"
#include "Shader.h"
#include "GameObject.h"
#include "Material.h"

#include <boost/random.hpp>

using namespace glm;

class Grass : public GameObject {
public:
	Grass() : GameObject() { };
	Grass(Material material);

protected:
	Vertex createVertex(vec3 position, vec3 normal, vec2 texCoords);
	shared_ptr<InstancedMesh> createMesh(shared_ptr<Shader> shader, vector<InstancedTransform> transforms);

	static boost::variate_generator<boost::mt19937, boost::uniform_real<>> randomGenerator;
};