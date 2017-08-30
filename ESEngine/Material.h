#ifndef MATERIAL_H
#define MATERIAL_H

#include "Texture.h"
#include "TextureManager.h"

#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <memory>

using namespace glm;

class Material {
public:
	vec3 ambient = vec3(0, 0, 0);
	vec3 diffuse = vec3(0, 0, 0);
	vec3 specular = vec3(0, 0, 0);
	GLfloat shininess = 0;

	std::shared_ptr<Texture> texDiffuse;
	std::shared_ptr<Texture> texSpecular;

	static Material cyanPlastic();
	static Material ruby();
	static Material obsidian();
	static Material brass();
	static Material copper();
	static Material greenRubber();
	static Material textured1();
	static Material textured2();
	static Material container();
	static Material grass();
	static Material leaves1();
	static Material leaves2();
	static Material leaves3();
	static Material stem1();
	static Material stem2();
	static Material bark1();
	static Material bark2();
	static Material bark3();
	static Material bark4();
	static Material bark5();

	static Material diffuseTextureOnly(string fileName);

};
#endif