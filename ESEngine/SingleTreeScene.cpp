#include "SingleTreeScene.h"

Logger SingleTreeScene::logger("SingleTreeScene");

void SingleTreeScene::initialize() {

	logger.log(INFO, "Starting scene");

	vector<string> treeParams = {
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
		"parametricTernaryTreeA.json"
		//15
	};

	vector<string> leavesParams = {
		"leaves1.png",
		"leaves2.png",
		"leaves3.png",
		"leaves4.png",
		"leaves5.png",
	};

	GameObject* go;
	Material barkMaterial;
	vec3 position = vec3(3,0,3);
	string paramsFileName;
	string leavesTextureName;
	bool normalVisualisation;
	bool meshWiring;

	paramsFileName = "LindenmayerRules/" + treeParams[14];
	leavesTextureName = "Textures/" + leavesParams[1];
	barkMaterial = Material::bark1();
	normalVisualisation = false;
	meshWiring = false;	
	go = createLindenmayerTree(paramsFileName, position, barkMaterial, Material::diffuseTextureOnly(leavesTextureName), meshWiring, normalVisualisation);

	generateTestBox(vec3(8, 2, 8), vec3(0.5, 0.5, 0.5));

	generateTerrain();
	addSkybox();

	//Light
	vec3 dir = normalize(vec3(-0.5, -0.5, 0));
	vec3 distance = vec3(-14);
	createDirectionalLight(distance * dir, dir);

	setActiveCamera(createCamera(vec3(0, 5, 15), -90, -10));

	generateFrameBuffer();
}

GameObject* SingleTreeScene::createLindenmayerTree(string paramsFileName, vec3 &position, Material &material, Material &leavesMaterial, bool meshWiring, bool normalVisualisation) {
	LindenmayerTreeParams params = LindenmayerTreeParams(paramsFileName);

	unique_ptr<GameObject> go = unique_ptr<GameObject>(new LindenmayerTree(params, material, leavesMaterial, meshWiring, normalVisualisation));
	go->name = "Tree";
	Transform *transform = getTransform(go.get());
	transform->translate(position);
	((LindenmayerTree*)go.get())->generate();

	shared_ptr<RigidBody> rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsAHullShape(0);

	return addGameObject(move(go));
}

void SingleTreeScene::generateTerrain() {
	Transform* transform;
	shared_ptr<RigidBody> rigidBody;
	unique_ptr<GameObject> go;

	go = unique_ptr<GameObject>(new Cube(Material::grass()));
	transform = getTransform(go.get());
	transform->scale(vec3(15, 0.5, 15));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(0);
	addGameObject(move(go));
}

GameObject* SingleTreeScene::createWhiteLampCube(vec3 position, PointLightStrength str) {
	vec4 color = vec4(1, 1, 1, 1);
	unique_ptr<GameObject> go(new LampCube(color));
	go->addComponent(shared_ptr<PointLight>(new PointLight(str)));
	Transform* transform = getTransform(go.get());
	transform->translate(position);
	transform->scale(vec3(.4, .4, .4));
	return addGameObject(move(go));
}

GameObject* SingleTreeScene::createDirectionalLight(vec3 position, vec3 directory) {
	DirectionalLight::setProjectionParams(20.0f, 50.0f);

	unique_ptr<GameObject> go(new GameObject());
	go->addComponent(shared_ptr<DirectionalLight>(new DirectionalLight()));
	go->name = "DirLight";
	Transform* transform = getTransform(go.get());
	transform->translate(position);
	DirectionalLight *light = (DirectionalLight*)go->getComponent(LIGHT);
	light->directory = directory;
	return addGameObject(move(go));
}

GameObject* SingleTreeScene::createCamera(glm::vec3 position, float yaw, float pitch) {
	unique_ptr<GameObject> go(new GameObject());
	shared_ptr<Camera> camera = shared_ptr<Camera>(new Camera(position, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch));
	go->addComponent(camera);
	shared_ptr<CameraBehaviour> cameraBehaviour = shared_ptr<CameraBehaviour>(new CameraBehaviour(camera.get()));
	go->addComponent(cameraBehaviour);
	return addGameObject(move(go));
}

void SingleTreeScene::generateTestBox(vec3 pos, vec3 scale = vec3(1, 1, 1)) {
	unique_ptr<GameObject> go = unique_ptr<GameObject>(new Cube(Material::container(), false, false));
	go->name = "CrateCube";
	Transform* transform = getTransform(go.get());
	transform->translate(pos);
	shared_ptr<RigidBody> rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1);
	rigidBody->makeDynamic();

	addGameObject(move(go));
}

void SingleTreeScene::addSkybox() {
	string skyboxTex[] = {
		"Textures/Skybox_darkforest/right.jpg",
		"Textures/Skybox_darkforest/left.jpg",
		"Textures/Skybox_darkforest/top.jpg",
		"Textures/Skybox_darkforest/bottom.jpg",
		"Textures/Skybox_darkforest/back.jpg",
		"Textures/Skybox_darkforest/front.jpg"
	};

	Shader shader("Shaders/SkyboxShader.vert", "Shaders/SkyboxShader.frag");

	unique_ptr<Skybox> skybox(new Skybox(skyboxTex, shader));
	setSkybox(std::move(skybox));
}

void SingleTreeScene::generateFrameBuffer() {
	setFrameBuffer(unique_ptr<FrameBuffer>(new HDRFrameBuffer()));
}