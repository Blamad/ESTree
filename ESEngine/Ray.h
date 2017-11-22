#ifndef RAY_H
#define RAY_H

#include <GLM/glm.hpp>

using namespace glm;

struct Ray {
	vec3 origin;
	vec3 end;
	vec3 dir;
};

#endif