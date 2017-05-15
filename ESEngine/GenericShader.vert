#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

out VS_OUT {
  vec3 fragPos;
  vec3 normal;
} vs_out;

layout (std140, binding = 0) uniform Matricies
{
	uniform mat4 projection;
	uniform mat4 view;
};

uniform mat4 model;
uniform mat3 normalModel;

void main()
{
	vec4 modelSpacePos = model * vec4(position, 1.0f);
	vec4 viewSpacePos = view * modelSpacePos;
	gl_Position = projection * viewSpacePos;

	vs_out.fragPos = vec3(model * vec4(position, 1.0f));
	vs_out.normal = normalModel * normal;
}