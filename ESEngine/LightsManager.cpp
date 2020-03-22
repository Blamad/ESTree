#include "LightsManager.h"

GLuint LightsManager::lightsUBO = 0;

LightsManager::LightsManager() {
	lights[LIGHT_POINT] = std::set<Light*>();
	lights[LIGHT_SPOT] = std::set<Light*>();
	lights[LIGHT_DIRECTIONAL] = std::set<Light*>();

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

void LightsManager::prepareShadowBuffer(glm::vec3& viewPos, Renderer& renderer, std::function<void(Renderer&, Shader*)> renderObjectsFunction) {
	glm::mat4 lightProjection = DirectionalLight::getProjectionMatrix();
	glm::mat4 lightView;

	Shader::updateProjectionMatrix(lightProjection);

	depthBuffer->mountFrameBuffer();
	depthBuffer->shader->setShaderSubroutine("shadowDepthPass");

	BOOST_FOREACH(Light* light, lights[LIGHT_DIRECTIONAL]) {
		DirectionalLight* dLight = (DirectionalLight*)light;
		glm::mat4 lightView = DirectionalLight::getViewMatrix(dLight);

		Shader::updateViewMatrix(lightView);
		dLight->lightSpace = lightProjection * lightView;
		renderObjectsFunction(renderer, depthBuffer->shader.get());
	}
	depthBuffer->shader->setShaderSubroutine("renderPass");
	depthBuffer->unmountFrameBuffer();
	Shader::updateProjectionMatrix(Camera::getProjectionMatrix());

	updateLightsUBO(viewPos);
}

void LightsManager::updateLightsUBO(glm::vec3 &viewPos) {
	LightsData data = LightsUtils::extractData(lights[LIGHT_POINT], lights[LIGHT_DIRECTIONAL], viewPos);

	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightsData), &data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	for (int i = 2; i < lights[LIGHT_DIRECTIONAL].size() + 2; i++) {
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
	float sizeMod = 4;
	std::shared_ptr<Shader> shader = ShaderManager::getShader("Shaders/GenericShader.vert", "Shaders/GenericShader.frag");
	shader->initializeMatricesUBO();
	depthBuffer = std::unique_ptr<DepthFrameBuffer>(new DepthFrameBuffer(shader, Screen::getScreenWidth() * sizeMod, Screen::getScreenHeight() * sizeMod));
}