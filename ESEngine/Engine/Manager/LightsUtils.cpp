#include "LightsUtils.h"

LightsData LightsUtils::extractData(std::set<Light*> &pLights, std::set<Light*> &dLights, glm::vec3 &viewPos) {
	LightsData data;
	data.viewPos = glm::vec4(viewPos, 0);
	for (const auto &node : pLights)
		data.pointData[data.pointLength++] = extractData(((PointLight*)node));
	for (const auto &node : dLights)
		data.directionalData[data.directionalLength++] = extractData(((DirectionalLight*)node));
	return data;
}

PointLightData LightsUtils::extractData(PointLight *light) {
	Transform *transform = (Transform*)light->getComponent(TRANSFORM);
	PointLightData data;
	data.position = glm::vec4(transform->getPosition(), 0);
	data.ambient = glm::vec4(light->ambient, 0);
	data.specular = glm::vec4(light->specular, 0);
	data.diffuse = glm::vec4(light->diffuse, 0);
	data.linear = light->linear;
	data.constant = light->constant;
	data.quadratic = light->quadratic;
	return data;
}

DirectionalLightData LightsUtils::extractData(DirectionalLight *light) {
	DirectionalLightData data;
	data.directory = glm::vec4(light->directory, 0);
	data.ambient = glm::vec4(light->ambient, 0);
	data.specular = glm::vec4(light->specular, 0);
	data.diffuse = glm::vec4(light->diffuse, 0);
	data.lightSpace = light->lightSpace;
	return data;
}