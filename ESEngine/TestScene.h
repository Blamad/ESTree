#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "Scene.h"
#include "Material.h"

class TestScene : public Scene {
public:
	TestScene() : Scene() {
		initialize();
	};

	void initialize();

	GameObject* createCube(Material material);
	GameObject* createRotatingCube(Material material);

	GameObject* createWhiteLampCube(vec3 position, PointLightStrength str);
	GameObject* createDirectionalLight(vec3 direction);

	void generateTerrain();

protected:
	Transform* getTransform(GameObject* gameObject) {
		return (Transform*)gameObject->getComponent(TRANSFORM);
	}
};

#endif