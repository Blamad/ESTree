#ifndef LIGHTSMANAGER_H
#define LIGHTSMANAGER_H

#include <map>
#include <set>
#include <memory>
#include <boost/foreach.hpp>

#include "Shader.h"
#include "Renderer.h"
#include "DepthFrameBuffer.h"
#include "Light.h"
#include "PointLight.h"
#include "Transform.h"
#include "LightsUtils.h"
#include "Camera.h"

using namespace std;

class LightsManager {
public:
	LightsManager();


	void updateLights(vec3& viewPos, Renderer& renderer, function<void(Renderer&, Shader&)> renderObjectsFunction);

	void addLight(Light* light);
	void removeLight(Light* light);
	static void initializeLightsUBO();

private:
	static GLuint lightsUBO;

	unique_ptr<FrameBuffer> depthBuffer;
	map<LightType, set<Light*>> lights;

	void initialize();
	void updateLightsUBO(vec3& viewPos);
};

#endif
