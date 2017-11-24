#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Light.h"
#include "Transform.h"

class DirectionalLight : public Light {
public:
	vec3 directory;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	DirectionalLight(vec3 direction = vec3(0,-1, 1), vec3 ambient = vec3(0.05f, 0.05f, 0.05f), vec3 diffuse = vec3(0.4f, 0.4f, 0.4f), vec3 specular = vec3(0.5f, 0.5f, 0.5f)) : directory(directory), ambient(ambient), diffuse(diffuse), specular(specular), Light(DIRECTIONAL) { };

	static mat4 getViewMatrix(DirectionalLight* dLight) {
		Transform *transform = (Transform*)dLight->getComponent(TRANSFORM);

		return lookAt(transform->getPosition(),
			transform->getPosition() + dLight->directory,
			vec3(0.0f, 1.0f, 0.0f));
	}

	static mat4 getProjectionMatrix() {
		return ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
	}
};

#endif