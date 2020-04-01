#version 430 core
layout (location = 0) in vec3 position;

out VS_OUT {
	vec3 texCoords;
} vs_out;

layout (std140, binding = 0) uniform Matricies
{
	uniform mat4 projection;
	uniform mat4 view;
};

uniform mat4 model;

void main()
{
	//vec4 pos = projection * view * vec4(position, 1.0);
    //vs_out.texCoords = position;
	//gl_Position = pos.xyww;
	
	vec4 modelSpacePos = model * vec4(position, 1.0f);
	vec4 viewSpacePos = view * modelSpacePos;
	gl_Position = (projection * viewSpacePos).xyww;
	vs_out.texCoords = position;
}