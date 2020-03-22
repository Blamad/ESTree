#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "ShaderManager.h"
#include "Scene.h"
#include "Material.h"
#include "Cube.h"
#include "LampCube.h"
#include "InstancedCube.h"
#include "RotationBehaviour.h"
#include "RigidBody.h"
#include "HDRFrameBuffer.h"

class TestScene : public Scene {
public:
	TestScene() : Scene() {
		initialize();
	};

	void initialize();

	GameObject* createCube(Material material);
	GameObject* createRotatingCube(Material material);

	GameObject* createWhiteLampCube(glm::vec3 position, PointLightStrength str);
	GameObject* createDirectionalLight(glm::vec3 position, glm::vec3 direction);

	void generateTerrain();
	GameObject * createCamera(glm::vec3 position, float yaw, float pitch);
	void generateSkybox();

	void generateFrameBuffer();

protected:
	Transform* getTransform(GameObject* gameObject) {
		return (Transform*)gameObject->getComponent(TRANSFORM);
	}
};

#endif