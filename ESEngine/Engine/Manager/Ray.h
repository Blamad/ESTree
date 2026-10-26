#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

struct Ray {
	glm::vec3 origin;
	glm::vec3 end;
	glm::vec3 dir;
};

#endif