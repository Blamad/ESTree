#include "Material.h"

Material Material::cyanPlastic() {
	Material material;
	material.ambient = vec3(0.0f, 0.1f, 0.06f);
	material.diffuse = vec3(0.0f, 0.50980392f, 0.50980392f);
	material.specular = vec3(0.50196078f, 0.50196078f, 0.50196078f);
	material.shininess = .25f * 180.0f;
	return material;
}