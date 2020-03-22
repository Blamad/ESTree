#ifndef LIGHTSMANAGER_H
#define LIGHTSMANAGER_H

#include <map>
#include <set>
#include <memory>
#include <boost/foreach.hpp>

#include "ShaderManager.h"
#include "Renderer.h"
#include "DepthFrameBuffer.h"
#include "Light.h"
#include "PointLight.h"
#include "Transform.h"
#include "LightsUtils.h"
#include "Camera.h"

class LightsManager {
public:
	LightsManager();

	void prepareShadowBuffer(glm::vec3& viewPos, Renderer& renderer, std::function<void(Renderer&, Shader*)> renderObjectsFunction);

	void addLight(Light* light);
	void removeLight(Light* light);
	static void initializeLightsUBO();

private:
	static GLuint lightsUBO;

	std::unique_ptr<DepthFrameBuffer> depthBuffer;
	std::map<LightType, std::set<Light*>> lights;

	void initialize();
	void updateLightsUBO(glm::vec3& viewPos);
};

#endif
