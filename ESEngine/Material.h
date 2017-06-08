#ifndef MATERIAL_H
#define MATERIAL_H

#include <GLFW\glfw3.h>
#include <glm/glm.hpp>

using namespace glm;

class Material {
public:
	vec3 ambient = vec3(0.25f, 0.20725f, 0.20725f);
	vec3 diffuse = vec3(1.0f, 0.829f, 0.829f);
	vec3 specular = vec3(0.296648f, 0.296648f, 0.296648f);
	GLfloat shininess = 11.264f;

	static Material cyanPlastic();
	static Material ruby();
	static Material obsidian();
	static Material brass();
	static Material copper();
};
#endif