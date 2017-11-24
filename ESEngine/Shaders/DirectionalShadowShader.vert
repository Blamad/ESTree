#version 430 core
layout (location = 0) in vec3 position;

uniform mat4 model;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    gl_Position = projection*view * model * vec4(position, 1.0f);
}