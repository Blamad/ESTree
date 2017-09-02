#ifndef TREE_SCENE_H
#define TREE_SCENE_H

#include "Scene.h"
#include "Cube.h"
#include "LampCube.h"
#include "LindenmayerTree.h"
#include "Material.h"

#include "Logger.h"

class TreeScene : public Scene {
public:
	TreeScene() : Scene() { 
		initialize();
	};

	void initialize();

	GameObject* createLindenmayerTree(string configurationFileName, vec3 &position, Material &material, Material &leavesMaterial);

	GameObject* createWhiteLampCube(vec3 position, PointLightStrength str);
	GameObject* createDirectionalLight(vec3 direction);

	void generateTerrain();

protected:
	static Logger logger;

	Transform* getTransform(GameObject* gameObject) {
		return (Transform*)gameObject->getComponent(TRANSFORM);
	}
};

#endif