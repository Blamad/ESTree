#ifndef SINGLE_TREE_SCENE_H
#define SINGLE_TREE_SCENE_H

#include "ShaderManager.h"
#include "Scene.h"
#include "InstancedCube.h"
#include "Cube.h"
#include "Terrain.h"
#include "LampCube.h"
#include "LindenmayerTree.h"
#include "Material.h"
#include "Camera.h"
#include "CameraBehaviour.h"
#include "HDRFrameBuffer.h"

#include "Logger.h"

class SingleTreeScene : public Scene {
public:
	SingleTreeScene() : Scene() {
		initialize();
	};

	void initialize();

	GameObject* createLindenmayerTree(string configurationFileName, vec3 &position, Material &material, Material &leavesMaterial, bool meshWiring = false, bool normalVisualisation = false);

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