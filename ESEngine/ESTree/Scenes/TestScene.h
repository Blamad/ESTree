#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "Engine/Manager/ShaderManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/GameObject/Material.h"
#include "ESTree/Models/Cube.h"
#include "ESTree/Models/LampCube.h"
#include "ESTree/Models/InstancedCube.h"
#include "ESTree/Models/RotationBehaviour.h"
#include "Engine/GameObject/RigidBody.h"
#include "GraphicEngine/Framebuffer/HDRFrameBuffer.h"

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