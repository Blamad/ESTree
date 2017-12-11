#ifndef TEST_SCENE_H
#define TEST_SCENE_H

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

	GameObject* createWhiteLampCube(vec3 position, PointLightStrength str);
	GameObject* createDirectionalLight(vec3 position, vec3 direction);

	void generateTerrain();
	void generateSkybox();

	void generateFrameBuffer();

protected:
	Transform* getTransform(GameObject* gameObject) {
		return (Transform*)gameObject->getComponent(TRANSFORM);
	}
};

#endif