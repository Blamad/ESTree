#include "LightsManager.h"
#include <iostream>

GLuint LightsManager::lightsUBO = 0;

LightsManager::LightsManager() {
	lights[POINT] = set<shared_ptr<Light>>();
	lights[SPOT] = set<shared_ptr<Light>>();
	lights[DIRECTIONAL] = set<shared_ptr<Light>>();
}

void LightsManager::addLight(shared_ptr<Light> light) {
	lights[light->lightType].insert(light);
}

void LightsManager::removeLight(shared_ptr<Light> light) {
	lights[light->lightType].erase(light);
}

void LightsManager::updateLights(vec3 &viewPos) {
	updatePointLights(viewPos);
}

void LightsManager::updatePointLights(vec3 &viewPos) {
	
	LightsData data = LightsUtils::extractData(lights[POINT], lights[DIRECTIONAL], viewPos);
	
	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightsData), &data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightsManager::initializeLightsUBO() {
	int bufferSize = sizeof(LightsData);
	glGenBuffers(1, &lightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, Shader::lightBlockBinding, lightsUBO);
}