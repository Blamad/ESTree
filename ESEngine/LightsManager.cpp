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

void LightsManager::updateLights(vec3& viewPos, Renderer& renderer, function<void(Renderer&, Shader*)> renderObjectsFunction) {
	mat4 lightProjection = DirectionalLight::getProjectionMatrix();
	mat4 lightView;

	Shader::updateProjectionMatrix(lightProjection);

	depthBuffer->mountFrameBuffer();
	
	BOOST_FOREACH(Light* light, lights[DIRECTIONAL]) {
		DirectionalLight* dLight = (DirectionalLight*)light;
		mat4 lightView = DirectionalLight::getViewMatrix(dLight);

		Shader::updateViewMatrix(lightView);
		dLight->lightSpace = lightProjection * lightView;

		renderObjectsFunction(renderer, depthBuffer->shader.get());
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

	for (int i = 2; i < lights[DIRECTIONAL].size() + 2; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, depthBuffer->getBuffer()->id);
	}
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
	float sizeMod = 6;
	shared_ptr<Shader> shader = ShaderManager::getShader("Shaders/DirectionalShadowShader.vert", "Shaders/DirectionalShadowShader.frag");
	shader->initializeMatricesUBO();
	depthBuffer = unique_ptr<DepthFrameBuffer>(new DepthFrameBuffer(shader, Screen::getScreenWidth() * sizeMod, Screen::getScreenHeight() * sizeMod));
}