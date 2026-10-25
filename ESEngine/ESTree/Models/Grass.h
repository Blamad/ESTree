#pragma once
#include "Engine/Manager/ShaderManager.h"
#include "Engine/GameObject/InstancedMesh.h"
#include "Engine/Core/Vertex.h"
#include "GraphicEngine/Core/Shader.h"
#include "Engine/GameObject/GameObject.h"
#include "Engine/GameObject/Material.h"

#include <ctime>
#include <random>

class Grass : public GameObject {
public:
	Grass() : GameObject() { };
	Grass(Material material);

protected:
	Vertex createVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords);
	std::shared_ptr<InstancedMesh> createMesh(std::shared_ptr<Shader> shader, std::vector<InstancedTransform> transforms);

	// Callable returning a uniform float in [-1, 1).
	struct RandomGenerator {
		std::mt19937 engine{ (unsigned int)time(0) };
		std::uniform_real_distribution<float> distribution{ -1.0f, 1.0f };
		float operator()() { return distribution(engine); }
	};
	static RandomGenerator randomGenerator;
};