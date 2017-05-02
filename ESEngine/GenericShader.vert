#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

out VS_OUT {
  vec4 Color;
  vec3 fragPosition;
  vec3 Normal;
} vs_out;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = vec4(position, 1.0f);//projection * view * model * vec4(position, 1.0f);
	//vs_out.fragPosition = vec3(model * vec4(position, 1.0f));
	//vs_out.Normal = mat3(transpose(inverse(model))) * normal;
	vs_out.Color = color;
}