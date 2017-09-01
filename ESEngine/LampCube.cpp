#include "LampCube.h"

LampCube::LampCube(vec4 color) : Cube() {
	Shader shader("LampShader.vert", "LampShader.frag");
	addComponent(createMesh(shader, color));
}