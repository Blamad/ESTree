#include "LightsManager.h"
#include <iostream>

GLuint LightsManager::lightsUBO = 0;

LightsManager::LightsManager() {
	lights[POINT] = set<Light*>();
	lights[SPOT] = set<Light*>();
	lights[DIRECTIONAL] = set<Light*>();
}

void LightsManager::addLight(Light* light) {
	lights[light->lightType].insert(light);
}

void LightsManager::removeLight(Light* light) {
	Light* removedLight;
	for (auto & l : lights[light->lightType])
		if (light == l) {
			removedLight = l;
			break;
		}
	lights[light->lightType].erase(removedLight);
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