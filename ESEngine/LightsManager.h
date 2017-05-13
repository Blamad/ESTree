#ifndef LIGHTSMANAGER_H
#define LIGHTSMANAGER_H

#include <map>
#include <set>
#include <memory>

#include "Shader.h"
#include "Light.h"
#include "PointLight.h"
#include "Transform.h"
#include "LightsUtils.h"

using namespace std;

class LightsManager {
public:
	LightsManager();

	void updateLights(glm::vec3 &viewPos);
	void addLight(shared_ptr<Light> light);
	void removeLight(shared_ptr<Light> light);
	static void initializeLightsUBO();

private:
	static GLuint lightsUBO;

	map<LightType, set<shared_ptr<Light>>> lights;
	void updatePointLights(glm::vec3 &viewPos);
};

#endif
