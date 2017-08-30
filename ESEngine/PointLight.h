#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"
#include "Transform.h"

enum PointLightStrength {
	WEAK, MEDIUM, STRONG
};

class PointLight : public Light {
public:
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;

	PointLight(GLfloat constant = 1.0f, GLfloat linear = 0.045f, GLfloat quadratic = 0.0075f, vec3 ambient = vec3(0.05f, 0.05f, 0.05f), vec3 diffuse = vec3(1, 1, 1), vec3 specular = vec3(1, 1, 1)) : ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadratic), Light(POINT) { };
	
	PointLight(PointLightStrength strength, vec3 ambient = vec3(0.05f, 0.05f, 0.05f), vec3 diffuse = vec3(1, 1, 1), vec3 specular = vec3(1, 1, 1)) : ambient(ambient), diffuse(diffuse), specular(specular), Light(POINT) {
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