#version 430 core

layout (location = 0) in vec3 position;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main()
{
	vec4 modelSpacePos = model * vec4(position, 1.0f);
	vec4 viewSpacePos = view * modelSpacePos;
	gl_Position = projection * viewSpacePos;
}