#version 430 core
layout (location = 0) in vec3 position;
/*layout (location = 3) in vec2 texCoords;

out VS_OUT {
  vec2 texCoords;
} vs_out;*/

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	//vs_out.texCoords = texCoords;
}