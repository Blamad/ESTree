#ifndef MATERIAL_H
#define MATERIAL_H

#include <GLFW\glfw3.h>
#include <glm/glm.hpp>

using namespace glm;

class Material {
public:
	vec3 ambient = vec3(0.05f, 0.0f, 0.0f);
	vec3 diffuse = vec3(0.5f, 0.4f, 0.4f);
	vec3 specular = vec3(0.7f, 0.04f, 0.04f);
	GLfloat shininess = 10.0f;

	static Material cyanPlastic();
};
#endif