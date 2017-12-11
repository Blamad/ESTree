#ifndef TREE_SCENE_H
#define TREE_SCENE_H

#include "Scene.h"
#include "Cube.h"
#include "LampCube.h"
#include "LindenmayerTree.h"
#include "Material.h"
#include "Camera.h"
#include "CameraBehaviour.h"
#include "HDRFrameBuffer.h"

#include "Logger.h"

class TreeScene : public Scene {
public:
	TreeScene() : Scene() { 
		initialize();
	};

	void initialize();

	GameObject* createLindenmayerTree(string configurationFileName, vec3 &position, Material &material, Material &leavesMaterial, bool debug = false);

	GameObject* createWhiteLampCube(vec3 position, PointLightStrength str);
	GameObject* createDirectionalLight(vec3 position, vec3 direction);
	GameObject* createCamera(glm::vec3 position, float yaw, float pitch);

	void generateTerrain();
	void addSkybox();

	void generateTestBox(vec3 pos, vec3 scale);
	void generateFrameBuffer();

protected:
	static Logger logger;

	Transform* getTransform(GameObject* gameObject) {
		return (Transform*)gameObject->getComponent(TRANSFORM);
	}
};

#endif