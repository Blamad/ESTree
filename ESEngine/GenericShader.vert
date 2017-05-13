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

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	vs_out.fragPos = vec3(model * vec4(position, 1.0f));
	//Jezeli skalowanie bylo nierownomierne to normalne sa kopniete. transpose(inverse(macierz)) ustawia normalne prostopadle do zamierzonej powierzchni
	//Zabieg mat3(macierz) obetnie nam translacje na modelu bo nie chcemy jej w obliczeniach swiatla
	vs_out.normal = mat3(transpose(inverse(model))) * normal;
}