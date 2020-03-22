#include "LampCube.h"

LampCube::LampCube(glm::vec4 color) : Cube() {
	std::shared_ptr<Shader> shader = ShaderManager::getShader("Shaders/LampShader.vert", "Shaders/LampShader.frag");
	addComponent(createMesh(shader, color));
}