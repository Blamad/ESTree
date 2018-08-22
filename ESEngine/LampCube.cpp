#include "LampCube.h"

LampCube::LampCube(vec4 color) : Cube() {
	shared_ptr<Shader> shader = ShaderManager::getShader("Shaders/LampShader.vert", "Shaders/LampShader.frag");
	addComponent(createMesh(shader, color));
}