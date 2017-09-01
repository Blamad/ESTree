#ifndef TREE_SCENE_H
#define TREE_SCENE_H

#include "Scene.h"
#include "LindenmayerTree.h"
#include "Material.h"

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
	Transform* getTransform(GameObject* gameObject) {
		return (Transform*)gameObject->getComponent(TRANSFORM);
	}
};

#endif