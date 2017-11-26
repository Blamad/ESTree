#include "LightsManager.h"

GLuint LightsManager::lightsUBO = 0;

LightsManager::LightsManager() {
	lights[POINT] = set<Light*>();
	lights[SPOT] = set<Light*>();
	lights[DIRECTIONAL] = set<Light*>();

	initialize();
}

void LightsManager::addLight(Light* light) {
	lights[light->lightType].insert(light);
}

void LightsManager::removeLight(Light* light) {
	Light* removedLight;
	for (auto & l : lights[light->lightType]) {
		if (light == l) {
			removedLight = l;
			break;
		}
	}
	lights[light->lightType].erase(removedLight);
}

void LightsManager::updateLights(vec3& viewPos, Renderer& renderer, function<void(Renderer&, Shader&)> renderObjectsFunction) {
	
	depthBuffer->mountFrameBuffer();
	Shader::updateProjectionMatrix(DirectionalLight::getProjectionMatrix());

	mat4 lightView;
	
	BOOST_FOREACH(Light* light, lights[DIRECTIONAL]) {
		DirectionalLight* dLight = (DirectionalLight*) light;
		lightView = DirectionalLight::getViewMatrix(dLight);
		
		Shader::updateViewMatrix(lightView);
		//renderObjectsFunction(renderer, *depthBuffer->shader);
	}

	depthBuffer->unmountFrameBuffer();
	Shader::updateProjectionMatrix(Camera::getProjectionMatrix());

	updateLightsUBO(viewPos);
}

void LightsManager::updateLightsUBO(vec3 &viewPos) {
	LightsData data = LightsUtils::extractData(lights[POINT], lights[DIRECTIONAL], viewPos);

	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightsData), &data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightsManager::initializeLightsUBO() {
	int bufferSize = sizeof(LightsData);
	glGenBuffers(1, &lightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, Shader::lightBlockBinding, lightsUBO);
}

void LightsManager::initialize() {
	unique_ptr<Shader> shader = unique_ptr<Shader>(new Shader("Shaders/DirectionalShadowShader.vert", "Shaders/DirectionalShadowShader.frag"));
	depthBuffer = unique_ptr<FrameBuffer>(new DepthFrameBuffer(move(shader), 2048, 2048));
}