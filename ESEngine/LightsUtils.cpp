#include "LightsUtils.h"

LightsData LightsUtils::extractData(set<shared_ptr<Light>> &pLights, set<shared_ptr<Light>> &dLights, glm::vec3 &viewPos) {
	LightsData data;
	data.viewPos = vec4(viewPos, 0);
	for (const auto &node : pLights)
		data.pointData[data.pointLength++] = extractData(((PointLight*)node.get()));
	for (const auto &node : dLights)
		data.directionalData[data.directionalLength++] = extractData(((DirectionalLight*)node.get()));
	return data;
}

PointLightData LightsUtils::extractData(PointLight *light) {
	Transform *transform = (Transform*)light->getComponent(TRANSFORM);
	PointLightData data;
	data.position =	vec4(transform->position, 0);
	data.ambient = vec4(light->ambient, 0);
	data.specular = vec4(light->specular, 0);
	data.diffuse =	vec4(light->diffuse, 0);
	data.linear = light->linear;
	data.constant = light->constant;
	data.quadratic = light->quadratic;
	return data;
}

DirectionalLightData LightsUtils::extractData(DirectionalLight *light) {
	DirectionalLightData data;
	data.directory = vec4(light->directory, 0);
	data.ambient = vec4(light->ambient, 0);
	data.specular = vec4(light->specular, 0);
	data.diffuse = vec4(light->diffuse, 0);
	return data;
}