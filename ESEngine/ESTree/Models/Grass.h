#pragma once
#include "Engine/Manager/ShaderManager.h"
#include "Engine/GameObject/InstancedMesh.h"
#include "Engine/Core/Vertex.h"
#include "GraphicEngine/Core/Shader.h"
#include "Engine/GameObject/GameObject.h"
#include "Engine/GameObject/Material.h"

#include <boost/random.hpp>

class Grass : public GameObject {
public:
	Grass() : GameObject() { };
	Grass(Material material);

protected:
	Vertex createVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords);
	std::shared_ptr<InstancedMesh> createMesh(std::shared_ptr<Shader> shader, std::vector<InstancedTransform> transforms);

	static boost::variate_generator<boost::mt19937, boost::uniform_real<>> randomGenerator;
};