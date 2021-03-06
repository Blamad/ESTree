#ifndef LIGHTSUTILS_H
#define LIGHTSUTILS_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <GLM\glm.hpp>
#include <set>
#include <memory>

#include "Engine/GameObject/PointLight.h"
#include "Engine/GameObject/DirectionalLight.h"

const int MAX_LIGHTS_SIZE = 10;

struct PointLightData {
	glm::vec4 position = glm::vec4(0);
	glm::vec4 ambient = glm::vec4(0);
	glm::vec4 diffuse = glm::vec4(0);
	glm::vec4 specular = glm::vec4(0);
	GLfloat constant = .0f;
	GLfloat linear = .0f;
	GLfloat quadratic = .0f;
	GLfloat padding = .0f;
};

struct DirectionalLightData {
	glm::vec4 directory = glm::vec4(0);
	glm::vec4 ambient = glm::vec4(0);
	glm::vec4 diffuse = glm::vec4(0);
	glm::vec4 specular = glm::vec4(0);
	glm::mat4 lightSpace = glm::mat4(1);
};

struct LightsData {
	PointLightData pointData[MAX_LIGHTS_SIZE];
	DirectionalLightData directionalData[MAX_LIGHTS_SIZE];
	glm::vec4 viewPos = glm::vec4(0);
	GLuint pointLength = 0;
	GLuint directionalLength = 0;
};

class LightsUtils {
public:
	static LightsData extractData(std::set<Light*> &pLights, std::set<Light*> &dLights, glm::vec3 &viewPos);
private:
	static PointLightData extractData(PointLight *light);
	static DirectionalLightData extractData(DirectionalLight *light);
};

#endif