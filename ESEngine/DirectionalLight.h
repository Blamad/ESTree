#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Light.h"
#include "Transform.h"

class DirectionalLight : public Light {
public:
	glm::vec3 directory;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::mat4 lightSpace;

	DirectionalLight(glm::vec3 direction = glm::vec3(0,-1, 1), glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3 diffuse = glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f)) : directory(directory), ambient(ambient), diffuse(diffuse), specular(specular), Light(LIGHT_DIRECTIONAL) { };

	static glm::mat4 getViewMatrix(DirectionalLight* dLight) {
		Transform *transform = (Transform*)dLight->getComponent(TRANSFORM);

		return lookAt(transform->getPosition(),
			dLight->directory,
			glm::vec3(0.0f, 1.0f, 0.0f));
	}

	static void setProjectionParams(float bufferSize, float farPane) {
		shadingBufferSize = bufferSize;
		shadingFarPane = farPane;
	}

	static glm::mat4 getProjectionMatrix() {
		return glm::ortho(-1 * shadingBufferSize, 1 * shadingBufferSize, -1 * shadingBufferSize, 1 * shadingBufferSize, 0.01f, shadingFarPane);
	}

private:
	static float shadingBufferSize;
	static float shadingFarPane;
};

#endif