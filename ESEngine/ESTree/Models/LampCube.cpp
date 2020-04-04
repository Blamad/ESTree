#include "LampCube.h"

LampCube::LampCube(glm::vec4 color) : Cube() {
	std::shared_ptr<Shader> shader = ShaderManager::getShader("Resources/Shaders/LampShader.vert", "Resources/Shaders/LampShader.frag");
	addComponent(createMesh(shader, color));
}