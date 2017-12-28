#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 texCoords;
layout(location = 4) in mat4 instancedModel;
layout(location = 8) in mat3 instancedNormalModel;

out VS_OUT {
  vec3 fragPos;
  vec3 normal;
  vec2 texCoords;
} vs_out;

layout (std140, binding = 0) uniform Matricies {
	uniform mat4 projection;
	uniform mat4 view;
};

uniform mat4 model;
uniform mat3 normalModel;

subroutine void renderMode();
subroutine uniform renderMode renderModeSubroutine;

subroutine (renderMode)
void singleMesh() {
	vec4 modelSpacePos = model * vec4(position, 1.0f);
	vec4 viewSpacePos = view * modelSpacePos;
	gl_Position = projection * viewSpacePos;

	vs_out.fragPos = vec3(model * vec4(position, 1.0f));
	vs_out.normal = normalModel * normal;
	vs_out.texCoords = texCoords;
}

subroutine (renderMode)
void instancedMesh() {
	vec4 modelSpacePos = instancedModel * vec4(position, 1.0f);
	vec4 viewSpacePos = view * modelSpacePos;
	gl_Position = projection * viewSpacePos;

	vs_out.fragPos = vec3(instancedModel * vec4(position, 1.0f));
	vs_out.normal = instancedNormalModel * normal;
	vs_out.texCoords = texCoords;
}

void main() {
	renderModeSubroutine();
}