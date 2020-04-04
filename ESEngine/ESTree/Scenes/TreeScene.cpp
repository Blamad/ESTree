#include "TreeScene.h"

Logger TreeScene::logger("TreeScene");

void TreeScene::initialize() {

	logger.log(LOG_INFO, "Starting scene");

	GameObject* go;
	glm::vec3 position;
	std::string paramsFileName;
	
	bool debug = false;
	std::string arrow = "Resources/Textures/arrow.png";

	paramsFileName = "Resources/LindenmayerRules/parametricSympodialTreeA.json";
	position = glm::vec3(20, 0, 20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark1(), Material::diffuseTextureOnly("Resources/Textures/leaves1.png"), "parametricSympodialTreeA", debug);

	paramsFileName = "Resources/LindenmayerRules/parametricSympodialTreeB.json";
	position = glm::vec3(20, 0, 7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark2(), Material::diffuseTextureOnly("Resources/Textures/leaves2.png"), "parametricSympodialTreeB", debug);

	paramsFileName = "Resources/LindenmayerRules/advancedTree2.json";
	position = glm::vec3(20, 0, -7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark3(), Material::diffuseTextureOnly("Resources/Textures/leaves3.png"), "advancedTree2", debug);

	paramsFileName = "Resources/LindenmayerRules/advancedTree2.json";
	position = glm::vec3(20, 0, -20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark4(), Material::diffuseTextureOnly("Resources/Textures/leaves4.png"), "advancedTree2", debug);

	paramsFileName = "Resources/LindenmayerRules/parametricMonopodialTreeA.json";
	position = glm::vec3(7.5, 0, 20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark2(), Material::diffuseTextureOnly("Resources/Textures/leaves2.png"), "parametricMonopodialTreeA", debug);

	paramsFileName = "Resources/LindenmayerRules/parametricMonopodialTreeB.json";
	position = glm::vec3(7.5, 0, 7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark2(), Material::diffuseTextureOnly("Resources/Textures/leaves2.png"), "parametricMonopodialTreeB", debug);

	paramsFileName = "Resources/LindenmayerRules/parametricMonopodialTreeC.json";
	position = glm::vec3(7.5, 0, -7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark3(), Material::diffuseTextureOnly("Resources/Textures/leaves3.png"), "parametricMonopodialTreeC", debug);

	paramsFileName = "Resources/LindenmayerRules/parametricMonopodialTreeD.json";
	position = glm::vec3(7.5, 0, -20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark4(), Material::diffuseTextureOnly("Resources/Textures/leaves4.png"), "parametricMonopodialTreeD", debug);
	
	paramsFileName = "Resources/LindenmayerRules/randomTree.json";
	position = glm::vec3(-7.5, 0, 20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark1(), Material::diffuseTextureOnly("Resources/Textures/leaves5.png"), "randomTree");

	paramsFileName = "Resources/LindenmayerRules/parametricTernaryTreeA.json";
	position = glm::vec3(-7.5, 0, 7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark5(), Material::leaves2(), "parametricTernaryTreeA");
	
	paramsFileName = "Resources/LindenmayerRules/symetricTree.json";
	position = glm::vec3(-7.5, 0, -7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark5(), Material::diffuseTextureOnly("Resources/Textures/leaves4.png"), "symetricTree");

	paramsFileName = "Resources/LindenmayerRules/fibbonacciTree.json";
	position = glm::vec3(-7.5, 0, -20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark5(), Material::leaves1(), "fibbonacciTree");

	paramsFileName = "Resources/LindenmayerRules/advancedTree2.json";
	position = glm::vec3(-20, 0, 20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark3(), Material::diffuseTextureOnly("Resources/Textures/leaves3.png"), "advancedTree2");

	paramsFileName = "Resources/LindenmayerRules/parametricTernaryTreeB.json";
	position = glm::vec3(-20, 0, 7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark4(), Material::diffuseTextureOnly("Resources/Textures/leaves5.png"), "parametricTernaryTreeB");

	paramsFileName = "Resources/LindenmayerRules/parametricTernaryTreeE.json";
	position = glm::vec3(-20, 0, -7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark3(), Material::diffuseTextureOnly("Resources/Textures/leaves1.png"), "parametricTernaryTreeE");

	paramsFileName = "Resources/LindenmayerRules/parametricTernaryTreeD.json";
	position = glm::vec3(-20, 0, -20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark1(), Material::diffuseTextureOnly("Resources/Textures/leaves3.png"), "parametricTernaryTreeD");

	generateTestBox(glm::vec3(0, 1, 0), glm::vec3(0.5, 0.5, 0.5));

	generateTerrain();
	addSkybox();

	float distance = 50;
	glm::vec3 dir = normalize(glm::vec3(-1, -0.5, -1));
	createDirectionalLight(glm::vec3(dir.x * -distance, dir.x * -distance, dir.z * -distance), dir);

	setActiveCamera(createCamera(glm::vec3(-20, 15, -35), 45, -10));

	generateFrameBuffer();
}

GameObject* TreeScene::createLindenmayerTree(std::string paramsFileName, glm::vec3 &position, Material &material, Material &leavesMaterial, std::string name, bool debug) {
	LindenmayerTreeParams params = LindenmayerTreeParams(paramsFileName);

	std::unique_ptr<GameObject> go = std::unique_ptr<GameObject>(new LindenmayerTree(params, material, leavesMaterial, debug));
	go->name = name;
	Transform *transform = getTransform(go.get());
	transform->translate(position);
	((LindenmayerTree*)go.get())->generate();

	std::shared_ptr<RigidBody> rigidBody = std::shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsAHullShape(0);

	return addGameObject(move(go));
}

void TreeScene::generateTerrain() {
	Transform* transform;
	std::shared_ptr<RigidBody> rigidBody;
	std::unique_ptr<GameObject> go;

	int segmentSize = 6;
	int segWidth = 6;
	int segHeight = 6;

	int segPosX = -1 * segmentSize * segWidth;
	int segPosZ = -1 * segmentSize * segHeight;

	for (int i = 0; i < segWidth; i++) {
		for (int j = 0; j < segHeight; j++) {
			go = std::unique_ptr<GameObject>(new Cube(Material::grass()));
			transform = getTransform(go.get());
			transform->translate(glm::vec3(segPosX + segmentSize * i * 2, 0, segPosZ + segmentSize * j * 2));
			transform->scale(glm::vec3(segmentSize, 0.1, segmentSize));
			rigidBody = std::shared_ptr<RigidBody>(new RigidBody());
			go->addComponent(rigidBody);
			rigidBody->initAsBox(0);
			addGameObject(move(go));
		}
	}
}

GameObject* TreeScene::createWhiteLampCube(glm::vec3 position, PointLightStrength str) {
	glm::vec4 color = glm::vec4(1, 1, 1, 1);
	std::unique_ptr<GameObject> go(new LampCube(color));
	go->addComponent(std::shared_ptr<PointLight>(new PointLight(str)));
	Transform* transform = getTransform(go.get());
	transform->translate(position);
	transform->scale(glm::vec3(.4, .4, .4));
	return addGameObject(move(go));
}

GameObject* TreeScene::createDirectionalLight(glm::vec3 position, glm::vec3 directory) {
	DirectionalLight::setProjectionParams(20.0f, 60.0f);
	
	std::unique_ptr<GameObject> go(new GameObject());
	go->addComponent(std::shared_ptr<DirectionalLight>(new DirectionalLight()));
	go->name = "DirLight";
	Transform* transform = getTransform(go.get());
	transform->translate(position);
	DirectionalLight *light = (DirectionalLight*)go->getComponent(LIGHT);
	light->directory = directory;
	return addGameObject(move(go));
}

GameObject* TreeScene::createCamera(glm::vec3 position, float yaw, float pitch) {
	std::unique_ptr<GameObject> go(new GameObject());
	std::shared_ptr<Camera> camera = std::shared_ptr<Camera>(new Camera(position, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch));
	go->addComponent(camera);
	std::shared_ptr<CameraBehaviour> cameraBehaviour = std::shared_ptr<CameraBehaviour>(new CameraBehaviour(camera.get()));
	go->addComponent(cameraBehaviour);
	return addGameObject(move(go));
}

void TreeScene::generateTestBox(glm::vec3 pos, glm::vec3 scale = glm::vec3(1,1,1)) {
	std::unique_ptr<GameObject> go = std::unique_ptr<GameObject>(new Cube(Material::container()));
	go->name = "CrateCube";
	Transform* transform = getTransform(go.get());
	transform->translate(pos);
	std::shared_ptr<RigidBody> rigidBody = std::shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1);
	rigidBody->makeDynamic();

	addGameObject(move(go));
}

void TreeScene::addSkybox() {
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

void TreeScene::generateFrameBuffer() {
	setFrameBuffer(std::unique_ptr<FrameBuffer>(new HDRFrameBuffer()));
}