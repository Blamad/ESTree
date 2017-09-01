#include "TreeScene.h"
#include "Cube.h"
#include "LampCube.h"

void TreeScene::initialize() {

	GameObject* go;
	vec3 position;
	string paramsFileName;

	paramsFileName = "fibbonacciTree.l";
	position = vec3(-15, 0, -15);
	go = createLindenmayerTree(paramsFileName, position, Material::bark5(), Material::leaves3());

	position = vec3(15, 0, -15);
	go = createLTree(paramsFileName, position, Material::bark5(), Material::leaves3());

	/*paramsFileName = "randomTree.l";
	position = vec3(15, 0, -15);
	go = createLindenmayerTree(paramsFileName, position, Material::bark5(), Material::diffuseTextureOnly("leaves4.png"));*/

	/*paramsFileName = "advancedTree.l";
	position = vec3(0, 0, -15);
	go = createLindenmayerTree(paramsFileName, position, Material::bark2(), Material::leaves1());

	paramsFileName = "advancedTree2.l";
	position = vec3(-15, 0, -5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark2(), Material::leaves1());

	paramsFileName = "advancedTree3.l";
	position = vec3(15, 0, -5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark5(), Material::diffuseTextureOnly("leaves4.png"));
	*/
	generateTerrain();

	//Light
	createWhiteLampCube(vec3(-15, 20, 0), MEDIUM);
	createWhiteLampCube(vec3(15, 20, 0), MEDIUM);
	createWhiteLampCube(vec3(0, -5, 0), WEAK);
	createDirectionalLight(vec3(0, -1, -1));
}

GameObject* TreeScene::createLTree(string paramsFileName, vec3 &position, Material &material, Material &leavesMaterial) {
	LindenmayerTreeParams params = LindenmayerTreeParams(paramsFileName);
	GameObject* go = addGameObject(unique_ptr<GameObject>(new LTree(params, material, leavesMaterial)));
	Transform *transform = getTransform(go);
	transform->translate(position);
	((LTree*)go)->generate();
	return go;
}

GameObject* TreeScene::createLindenmayerTree(string paramsFileName, vec3 &position, Material &material, Material &leavesMaterial) {
	LindenmayerTreeParams params = LindenmayerTreeParams(paramsFileName);
	GameObject* go = addGameObject(unique_ptr<GameObject>(new LindenmayerTree(params, material, leavesMaterial)));
	Transform *transform = getTransform(go);
	transform->translate(position);
	((LindenmayerTree*)go)->generate();
	return go;
}

void TreeScene::generateTerrain() {
	Transform* transform;
	shared_ptr<RigidBody> rigidBody;
	unique_ptr<GameObject> go;

	int segmentSize = 6;
	int segWidth = 6;
	int segHeight = 6;

	int segPosX = -1 * segmentSize * segWidth;
	int segPosZ = -1 * segmentSize * segHeight;

	for (int i = 0; i < segWidth; i++) {
		for (int j = 0; j < segHeight; j++) {
			go = unique_ptr<GameObject>(new Cube(Material::grass()));
			transform = getTransform(go.get());
			transform->translate(vec3(segPosX + segmentSize * i * 2, 0, segPosZ + segmentSize * j * 2));
			transform->scale(vec3(segmentSize, 0.1, segmentSize));
			rigidBody = shared_ptr<RigidBody>(new RigidBody());
			go->addComponent(rigidBody);
			rigidBody->initAsBox(0, transform->getScale());
			addGameObject(move(go));
		}
	}
}

GameObject* TreeScene::createWhiteLampCube(vec3 position, PointLightStrength str) {
	vec4 color = vec4(1, 1, 1, 1);
	unique_ptr<GameObject> go(new LampCube(color));
	go->addComponent(shared_ptr<PointLight>(new PointLight(str)));
	Transform* transform = getTransform(go.get());
	transform->translate(position);
	transform->scale(vec3(.4, .4, .4));
	return addGameObject(move(go));
}

GameObject* TreeScene::createDirectionalLight(vec3 directory) {
	unique_ptr<GameObject> go(new GameObject());
	go->addComponent(shared_ptr<DirectionalLight>(new DirectionalLight()));
	DirectionalLight *light = (DirectionalLight*)go->getComponent(LIGHT);
	light->directory = directory;
	return addGameObject(move(go));
}