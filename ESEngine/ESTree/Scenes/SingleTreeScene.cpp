#include "SingleTreeScene.h"

Logger SingleTreeScene::logger("SingleTreeScene");

void SingleTreeScene::initialize() {

	logger.log(LOG_INFO, "Starting scene");

	std::vector<std::string> treeParams = {
		"randomTree.json",
		"symetricTree.json",
		"fibbonacciTree.json",
		"advancedTree.json",
		"advancedTree2.json",
		//5
		"test.json",
		"parametricMonopodialTreeA.json",
		"parametricMonopodialTreeB.json",
		"parametricMonopodialTreeC.json",
		"parametricMonopodialTreeD.json",
		//10
		"parametricSympodialTreeA.json",
		"parametricSympodialTreeB.json",
		"parametricSympodialTreeC.json",
		"parametricSympodialTreeD.json",
		"parametricTernaryTreeA.json",
		//15
		"parametricTernaryTreeB.json",
		"parametricTernaryTreeC.json",
		"parametricTernaryTreeD.json",
		"parametricTernaryTreeE.json",
		"test.json",
		//20
		"test2.json"
	};

	std::vector<std::string> leavesParams = {
		"leaves1.png",
		"leaves2.png",
		"leaves3.png",
		"leaves4.png",
		"leaves5.png",
		"arrow.png"
	};

	GameObject* go;
	Material barkMaterial;
	glm::vec3 position = glm::vec3(-0,0,0);
	std::string paramsFileName;
	std::string leavesTextureName;
	bool normalVisualisation;
	bool meshWiring;

	paramsFileName = "Resources/LindenmayerRules/" + treeParams[15];
	leavesTextureName = "Resources/Textures/" + leavesParams[2];
	barkMaterial = Material::bark3();
	normalVisualisation = false;
	meshWiring = false;

	
	position = glm:: vec3(0, 0, 0);
	go = createLindenmayerTree(paramsFileName, position, barkMaterial, Material::diffuseTextureOnly(leavesTextureName), meshWiring, normalVisualisation);

	generateTestBox(glm::vec3(8, 2, 8), glm::vec3(0.5, 0.5, 0.5));
	generateTestBox(glm::vec3(8, 4, 8), glm::vec3(0.5, 0.5, 0.5));

	generateGrass();
	generateTerrain();
	addSkybox();

	//Light
	glm::vec3 dir = normalize(glm::vec3(0, -0.5, -0.5));
	glm::vec3 distance = glm::vec3(-20);
	createDirectionalLight(distance * dir, dir);

	setActiveCamera(createCamera(glm::vec3(0, 5, 20), -90, 2));

	generateFrameBuffer();
}

GameObject* SingleTreeScene::createLindenmayerTree(std::string paramsFileName, glm::vec3 &position, Material &material, Material &leavesMaterial, bool meshWiring, bool normalVisualisation) {
	LindenmayerTreeParams params = LindenmayerTreeParams(paramsFileName);

	std::unique_ptr<GameObject> go = std::unique_ptr<GameObject>(new LindenmayerTree(params, material, leavesMaterial, meshWiring, normalVisualisation));
	go->name = "Tree";
	Transform *transform = getTransform(go.get());
	transform->translate(position);
	((LindenmayerTree*)go.get())->generate();

	std::shared_ptr<RigidBody> rigidBody = std::shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsAHullShape(0);

	return addGameObject(move(go));
}

void SingleTreeScene::generateGrass() {
	std::unique_ptr<GameObject> go = std::unique_ptr<GameObject>(new Grass(Material::diffuseTextureOnly("Resources/Textures/grassBlades2.png")));
	addGameObject(move(go));
}

void SingleTreeScene::generateTerrain() {
	Transform* transform;
	std::shared_ptr<RigidBody> rigidBody;
	std::unique_ptr<GameObject> go;

	go = std::unique_ptr<GameObject>(new Cube(Material::grass()));
	transform = getTransform(go.get());
	transform->scale(glm::vec3(21, 0.1, 21));
	rigidBody = std::shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(0);
	addGameObject(move(go));
}

GameObject* SingleTreeScene::createWhiteLampCube(glm::vec3 position, PointLightStrength str) {
	glm::vec4 color = glm::vec4(1, 1, 1, 1);
	std::unique_ptr<GameObject> go(new LampCube(color));
	go->addComponent(std::shared_ptr<PointLight>(new PointLight(str)));
	Transform* transform = getTransform(go.get());
	transform->translate(position);
	transform->scale(glm::vec3(.4, .4, .4));
	return addGameObject(move(go));
}

GameObject* SingleTreeScene::createDirectionalLight(glm::vec3 position, glm::vec3 directory) {
	DirectionalLight::setProjectionParams(20.0f, 50.0f);

	std::unique_ptr<GameObject> go(new GameObject());
	go->addComponent(std::shared_ptr<DirectionalLight>(new DirectionalLight()));
	go->name = "DirLight";
	Transform* transform = getTransform(go.get());
	transform->translate(position);
	DirectionalLight *light = (DirectionalLight*)go->getComponent(LIGHT);
	light->directory = directory;
	return addGameObject(move(go));
}

GameObject* SingleTreeScene::createCamera(glm::vec3 position, float yaw, float pitch) {
	std::unique_ptr<GameObject> go(new GameObject());
	std::shared_ptr<Camera> camera = std::shared_ptr<Camera>(new Camera(position, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch));
	go->addComponent(camera);
	std::shared_ptr<CameraBehaviour> cameraBehaviour = std::shared_ptr<CameraBehaviour>(new CameraBehaviour(camera.get()));
	go->addComponent(cameraBehaviour);
	return addGameObject(move(go));
}

void SingleTreeScene::generateTestBox(glm::vec3 pos, glm::vec3 scale = glm::vec3(1, 1, 1)) {
	std::unique_ptr<GameObject> go = std::unique_ptr<GameObject>(new Cube(Material::container(), false, false));
	go->name = "CrateCube";
	Transform* transform = getTransform(go.get());
	transform->translate(pos);
	std::shared_ptr<RigidBody> rigidBody = std::shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1);
	rigidBody->makeDynamic();

	addGameObject(move(go));
}

void SingleTreeScene::addSkybox() {
	std::string skyboxTex[] = {
		"Resources/Textures/Skybox_darkforest/right.jpg",
		"Resources/Textures/Skybox_darkforest/left.jpg",
		"Resources/Textures/Skybox_darkforest/top.jpg",
		"Resources/Textures/Skybox_darkforest/bottom.jpg",
		"Resources/Textures/Skybox_darkforest/back.jpg",
		"Resources/Textures/Skybox_darkforest/front.jpg"
	};

	std::unique_ptr<Skybox> skybox(new Skybox(skyboxTex));
	setSkybox(std::move(skybox));
}

void SingleTreeScene::generateFrameBuffer() {
	setFrameBuffer(std::unique_ptr<FrameBuffer>(new HDRFrameBuffer()));
}