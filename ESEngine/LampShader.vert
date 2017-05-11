#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

out VS_OUT {
  vec4 Color;
} vs_out;

layout (std140) uniform Matricies
{
	uniform mat4 projection;
	uniform mat4 view;
};

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
	vs_out.Color = color;
} 