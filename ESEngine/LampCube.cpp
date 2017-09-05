#include "LampCube.h"

LampCube::LampCube(vec4 color) : Cube() {
	Shader shader("Shaders/LampShader.vert", "Shaders/LampShader.frag");
	addComponent(createMesh(shader, color));
}