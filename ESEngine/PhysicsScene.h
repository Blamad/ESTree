#ifndef PHYSICS_SCENE_H
#define PHYSICS_SCENE_H

#include "Scene.h"
#include "Cube.h"
#include "Material.h"
#include "Camera.h"
#include "CameraBehaviour.h"
#include "HDRFrameBuffer.h"

#include "Logger.h"

class PhysicsScene : public Scene {
public:
	PhysicsScene() : Scene() {
		initialize();
	};

	void initialize();

	GameObject* createDirectionalLight(glm::vec3 position, glm::vec3 direction);
	GameObject* createCamera(glm::vec3 position, float yaw, float pitch);

	void generateTerrain();
	void addSkybox();

	void generateTower(glm::vec3 center, int floors);
	void generateFloor(glm::vec3 center);
	void generateBlock(glm::vec3 pos, bool rotate);
	void generateTestBox(glm::vec3 pos, glm::vec3 scale);
	void generateFrameBuffer();

protected:
	static Logger logger;

	Transform* getTransform(GameObject* gameObject) {
		return (Transform*)gameObject->getComponent(TRANSFORM);
	}

	float toRadians(float angle) {
		return angle * 3.14159265359 / 180;
	}

	float toAngle(float radians) {
		return radians * 180 / 3.14159265359;
	}
};

#endif