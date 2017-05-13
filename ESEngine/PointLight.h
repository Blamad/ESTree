#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"
#include "Transform.h"

class PointLight : public Light {
public:
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;

	PointLight(vec3 ambient = vec3(0.05f, 0.05f, 0.05f), vec3 diffuse = vec3(0.8f, 0.8f, 0.8f), vec3 specular = vec3(1, 1, 1), GLfloat constant = 1.0f, GLfloat linear = 0.09f, GLfloat quadratic = 0.032f) : ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadratic), Light(POINT) { };
};

#endif