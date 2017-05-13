#ifndef LIGHTSUTILS_H
#define LIGHTSUTILS_H

#include <GLFW\glfw3.h>
#include <GLM\glm.hpp>
#include <set>
#include <memory>

#include "PointLight.h"
#include "DirectionalLight.h"

using namespace std;

const int MAX_LIGHTS_SIZE = 10;

struct PointLightData {
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;
	GLfloat padding;
};

struct DirectionalLightData {
	vec4 directory;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

struct LightsData {
	PointLightData pointData[MAX_LIGHTS_SIZE];
	DirectionalLightData directionalData[MAX_LIGHTS_SIZE];
	vec4 viewPos;
	GLuint pointLength = 0;
	GLuint directionalLength = 0;
};

class LightsUtils {
public:
	static LightsData extractData(set<shared_ptr<Light>> &pLights, set<shared_ptr<Light>> &dLights, glm::vec3 &viewPos);
private:
	static PointLightData extractData(PointLight *light);
	static DirectionalLightData extractData(DirectionalLight *light);
};

#endif