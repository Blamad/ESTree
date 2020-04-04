#ifndef SINGLE_TREE_SCENE_H
#define SINGLE_TREE_SCENE_H

#include "Engine/Manager/ShaderManager.h"
#include "Engine/Scene/Scene.h"
#include "ESTree/Models/InstancedCube.h"
#include "ESTree/Models/Cube.h"
#include "ESTree/Models/Terrain.h"
#include "ESTree/Models/LampCube.h"
#include "ESTree/Models/Tree/LindenmayerTree.h"
#include "Engine/GameObject/Material.h"
#include "Engine/GameObject/Camera.h"
#include "Engine/GameObject/CameraBehaviour.h"
#include "GraphicEngine/Framebuffer/HDRFrameBuffer.h"
#include "ESTree/Models/Grass.h"

#include "Engine/Core/Logger.h"

class SingleTreeScene : public Scene {
public:
	SingleTreeScene() : Scene() {
		initialize();
	};

	void initialize();

	GameObject* createLindenmayerTree(std::string configurationFileName, glm::vec3 &position, Material &material, Material &leavesMaterial, bool meshWiring = false, bool normalVisualisation = false);

	GameObject* createWhiteLampCube(glm::vec3 position, PointLightStrength str);
	GameObject* createDirectionalLight(glm::vec3 position, glm::vec3 direction);
	GameObject* createCamera(glm::vec3 position, float yaw, float pitch);

	void generateTerrain();
	void generateGrass();
	void addSkybox();

	void generateTestBox(glm::vec3 pos, glm::vec3 scale);
	void generateFrameBuffer();

protected:
	static Logger logger;

	Transform* getTransform(GameObject* gameObject) {
		return (Transform*)gameObject->getComponent(TRANSFORM);
	}
};

#endif