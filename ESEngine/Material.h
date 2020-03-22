#ifndef MATERIAL_H
#define MATERIAL_H

#include "Texture.h"
#include "TextureManager.h"

#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <memory>

class Material {
public:
	glm::vec3 ambient = glm::vec3(0, 0, 0);
	glm::vec3 diffuse = glm::vec3(0, 0, 0);
	glm::vec3 specular = glm::vec3(0, 0, 0);
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
	static Material bark1();
	static Material bark2();
	static Material bark3();
	static Material bark4();
	static Material bark5();

	static Material diffuseTextureOnly(std::string fileName);

	bool operator<(Material const& r) const {
		Material l = *this;

		if (l.texDiffuse != nullptr && r.texDiffuse == nullptr) return true;
		if (l.texDiffuse == nullptr && r.texDiffuse != nullptr) return false;
		if (l.texDiffuse > r.texDiffuse) return true;
		if (l.texDiffuse < r.texDiffuse) return false;

		if (l.texSpecular != nullptr && r.texSpecular == nullptr) return true;
		if (l.texSpecular == nullptr && r.texSpecular != nullptr) return false;
		if (l.texSpecular > r.texSpecular) return true;
		if (l.texSpecular < r.texSpecular) return false;

		if (l.diffuse.length() < r.diffuse.length())  return true;
		if (l.diffuse.length() > r.diffuse.length())  return false;

		if (l.specular.length() < r.specular.length())  return true;
		if (l.specular.length() > r.specular.length())  return false;

		// Otherwise both are equal
		return false;
	}
};
#endif