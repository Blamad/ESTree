#ifndef LIGHTSMANAGER_H
#define LIGHTSMANAGER_H

#include <map>
#include <set>
#include <memory>
#include <boost/foreach.hpp>

#include "ShaderManager.h"
#include "GraphicEngine/Core/Renderer.h"
#include "GraphicEngine/Framebuffer/DepthFrameBuffer.h"
#include "Engine/GameObject/Light.h"
#include "Engine/GameObject/PointLight.h"
#include "Engine/GameObject/Transform.h"
#include "Engine/Manager/LightsUtils.h"
#include "Engine/GameObject/Camera.h"

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
