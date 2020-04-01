#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out VS_OUT {
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
    gl_Position = projection * view * model * vec4(position, 1.0); 
    vs_out.normal = mat3(transpose(inverse(view))) * normal;
}