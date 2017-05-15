#include "Material.h"

Material Material::cyanPlastic() {
	Material material;
	material.ambient = vec3(0.0f, 0.1f, 0.06f);
	material.diffuse = vec3(0.0f, 0.50980392f, 0.50980392f);
	material.specular = vec3(0.50196078f, 0.50196078f, 0.50196078f);
	material.shininess = .25f * 180.0f;
	return material;
}

Material Material::ruby() {
	Material material;
	material.ambient = vec3(0.1745f, 0.01175f, 0.01175f);
	material.diffuse = vec3(0.61424f, 0.04136f, 0.04136f);
	material.specular = vec3(0.727811f, 0.626959f, 0.626959f);
	material.shininess = 76.8f;
	return material;
}