#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"
#include "Transform.h"

enum PointLightStrength {
	WEAK, MEDIUM, STRONG
};

class PointLight : public Light {
public:
	glm::vec3 ambient = glm::vec3();
	glm::vec3 diffuse = glm::vec3();
	glm::vec3 specular = glm::vec3();
	GLfloat constant = .0f;
	GLfloat linear = .0f;
	GLfloat quadratic = .0f;

	PointLight(GLfloat constant = 1.0f, GLfloat linear = 0.045f, GLfloat quadratic = 0.0075f, glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3 diffuse = glm::vec3(1, 1, 1), glm::vec3 specular = glm::vec3(1, 1, 1)) : ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadratic), Light(LIGHT_POINT) { };
	
	PointLight(PointLightStrength strength, glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3 diffuse = glm::vec3(1, 1, 1), glm::vec3 specular = glm::vec3(1, 1, 1)) : ambient(ambient), diffuse(diffuse), specular(specular), Light(LIGHT_POINT) {
		switch (strength) {
			constant = 1;
		case WEAK:
			linear = 0.045;
			quadratic = 0.0075;
			break;
		case MEDIUM:
			linear = 0.022;
			quadratic = 0.0019;
			break;
		case STRONG:
			linear = 0.007;
			quadratic = 0.0002;
			break;
		}
	};
};
#endif