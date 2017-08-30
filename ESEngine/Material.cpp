#include "Material.h"

Material Material::cyanPlastic() {
	Material material;
	material.ambient = vec3(0.0f, 0.1f, 0.06f);
	material.diffuse = vec3(0.0f, 0.50980392f, 0.50980392f);
	material.specular = vec3(0.50196078f, 0.50196078f, 0.50196078f);
	material.shininess = 32.0f;
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

Material Material::obsidian() {
	Material material;
	material.ambient = vec3(0.05375f, 0.05f, 0.06625f);
	material.diffuse = vec3(0.18275f, 0.17f, 0.22525f);
	material.specular = vec3(0.332741f, 0.328634f, 0.346435f);
	material.shininess = 38.4f;
	return material;
}

Material Material::brass() {
	Material material;
	material.ambient = vec3(0.329412f, 0.223529f, 0.027451f);
	material.diffuse = vec3(0.780392f, 0.568627f, 0.113725f);
	material.specular = vec3(0.992157f, 0.941176f, 0.807843f);
	material.shininess = 27.8974f;
	return material;
}

Material Material::copper() {
	Material material;
	material.ambient = vec3(0.19125f, 0.0735f, 0.0225f);
	material.diffuse = vec3(0.7038f, 0.27048f, 0.0828f);
	material.specular = vec3(0.256777f, 0.137622f, 0.086014f);
	material.shininess = 12.8f;
	return material;
}

Material Material::greenRubber() {
	Material material;
	material.ambient = vec3(0, 0.05, 0);
	material.diffuse = vec3(0.4, 0.5, 0.4);
	material.specular = vec3(0.04, 0.7, 0.04);
	material.shininess = .078125 * 128;
	return material;
}

Material Material::textured1() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("treeTexture1.jpg", DIFFUSE);
	material.shininess = 64.0f;
	return material;
}

Material Material::textured2() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("treeTexture2.jpg", DIFFUSE);
	material.shininess = 64.0f;
	return material;
}

Material Material::grass() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("grassTexture.jpg", DIFFUSE);
	material.shininess = 64.0f;
	return material;
}

Material Material::container() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("container_d.jpg", DIFFUSE);
	material.texSpecular = TextureManager::getInstance().createTexture("container_s.jpg", SPECULAR);
	material.shininess = 64.0f;
	return material;
}

Material Material::leaves1() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("leaves1.png", DIFFUSE);
	material.shininess = 64.0f;
	return material;
}

Material Material::leaves2() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("leaves2.png", DIFFUSE);
	material.shininess = 64.0f;
	return material;
}

Material Material::leaves3() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("leaves3.png", DIFFUSE);
	material.shininess = 64.0f;
	return material;
}

Material Material::stem1() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("stem1.png", DIFFUSE);
	material.shininess = 64.0f;
	return material;
}

Material Material::stem2() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("stem2.png", DIFFUSE);
	material.shininess = 64.0f;
	return material;
}

Material Material::bark1() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("barkTexture1.jpg", DIFFUSE);
	material.shininess = 64.0f;
	return material;
}

Material Material::bark2() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("barkTexture2.jpg", DIFFUSE);
	material.shininess = 64.0f;
	return material;
}

Material Material::bark3() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("barkTexture3.jpg", DIFFUSE);
	material.shininess = 64.0f;
	return material;
}

Material Material::bark4() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("barkTexture4.jpg", DIFFUSE);
	material.shininess = 64.0f;
	return material;
}

Material Material::bark5() {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture("barkTexture5.jpg", DIFFUSE);
	material.shininess = 64.0f;
	return material;
}

static Material diffuseTextureOnly(string fileName) {
	Material material;
	material.texDiffuse = TextureManager::getInstance().createTexture(fileName, DIFFUSE);
	material.shininess = 64.0f;
	return material;
}