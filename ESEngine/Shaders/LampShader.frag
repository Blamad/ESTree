#version 330 core
out vec4 color;

in VS_OUT {
  vec4 Color;
} vs_in;

void main()
{
	color = vs_in.Color;
}