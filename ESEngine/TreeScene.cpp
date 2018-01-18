#include "TreeScene.h"

Logger TreeScene::logger("TreeScene");

void TreeScene::initialize() {

	logger.log(INFO, "Starting scene");

	GameObject* go;
	vec3 position;
	string paramsFileName;
	
	bool debug = true;

	paramsFileName = "LindenmayerRules/parametricSympodialTreeA.json";
	position = vec3(20, 0, 20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark1(), Material::diffuseTextureOnly("Textures/leaves1.png"), debug);

	paramsFileName = "LindenmayerRules/parametricSympodialTreeB.json";
	position = vec3(20, 0, 7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark2(), Material::diffuseTextureOnly("Textures/leaves2.png"), debug);

	paramsFileName = "LindenmayerRules/parametricSympodialTreeC.json";
	position = vec3(20, 0, -7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark3(), Material::diffuseTextureOnly("Textures/leaves3.png"), debug);

	paramsFileName = "LindenmayerRules/parametricSympodialTreeD.json";
	position = vec3(20, 0, -20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark4(), Material::diffuseTextureOnly("Textures/leaves4.png"), debug);

	paramsFileName = "LindenmayerRules/parametricMonopodialTreeA.json";
	position = vec3(7.5, 0, 20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark2(), Material::diffuseTextureOnly("Textures/leaves2.png"), debug);

	paramsFileName = "LindenmayerRules/parametricMonopodialTreeB.json";
	position = vec3(7.5, 0, 7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark2(), Material::diffuseTextureOnly("Textures/leaves2.png"), debug);

	paramsFileName = "LindenmayerRules/parametricMonopodialTreeC.json";
	position = vec3(7.5, 0, -7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark3(), Material::diffuseTextureOnly("Textures/leaves3.png"), debug);

	/*paramsFileName = "LindenmayerRules/parametricMonopodialTreeD.json";
	position = vec3(7.5, 0, -20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark4(), Material::diffuseTextureOnly("Textures/leaves4.png"), debug);
	
	paramsFileName = "LindenmayerRules/randomTree.json";
	position = vec3(-7.5, 0, 20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark1(), Material::diffuseTextureOnly("Textures/leaves5.png"));

	paramsFileName = "LindenmayerRules/randomTree.json";
	position = vec3(-7.5, 0, 7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark5(), Material::leaves2());
	
	paramsFileName = "LindenmayerRules/symetricTree.json";
	position = vec3(-7.5, 0, -7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark5(), Material::diffuseTextureOnly("Textures/leaves4.png"));

	paramsFileName = "LindenmayerRules/fibbonacciTree.json";
	position = vec3(-7.5, 0, -20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark5(), Material::leaves1());

	paramsFileName = "LindenmayerRules/advancedTree2.json";
	position = vec3(-20, 0, 20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark3(), Material::diffuseTextureOnly("Textures/leaves3.png"));

	paramsFileName = "LindenmayerRules/advancedTree2.json";
	position = vec3(-20, 0, 7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark4(), Material::diffuseTextureOnly("Textures/leaves5.png"));

	paramsFileName = "LindenmayerRules/advancedTree2.json";
	position = vec3(-20, 0, -7.5);
	go = createLindenmayerTree(paramsFileName, position, Material::bark3(), Material::diffuseTextureOnly("Textures/leaves1.png"));

	paramsFileName = "LindenmayerRules/advancedTree2.json";
	position = vec3(-20, 0, -20);
	go = createLindenmayerTree(paramsFileName, position, Material::bark1(), Material::diffuseTextureOnly("Textures/leaves3.png"));*/

	/**/
	generateTestBox(vec3(0, 1, 0), vec3(0.5, 0.5, 0.5));

	generateTerrain();
	addSkybox();

	vec3 dir = normalize(vec3(-1, -0.5, -1));
	createDirectionalLight(vec3(dir.x * -10, 15, dir.z * -10), dir);

	setActiveCamera(createCamera(vec3(-20, 15, -35), 45, -10));

	generateFrameBuffer();
}

GameObject* TreeScene::createLindenmayerTree(string paramsFileName, vec3 &position, Material &material, Material &leavesMaterial, bool debug) {
	LindenmayerTreeParams params = LindenmayerTreeParams(paramsFileName);

	unique_ptr<GameObject> go = unique_ptr<GameObject>(new LindenmayerTree(params, material, leavesMaterial, debug));
	go->name = "Tree";
	Transform *transform = getTransform(go.get());
	transform->translate(position);
	((LindenmayerTree*)go.get())->generate();

	shared_ptr<RigidBody> rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsAHullShape(0);

	return addGameObject(move(go));
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
			rigidBody->initAsBox(0);
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

GameObject* TreeScene::createDirectionalLight(vec3 position, vec3 directory) {
	DirectionalLight::setProjectionParams(20.0f, 60.0f);
	
	unique_ptr<GameObject> go(new GameObject());
	go->addComponent(shared_ptr<DirectionalLight>(new DirectionalLight()));
	go->name = "DirLight";
	Transform* transform = getTransform(go.get());
	transform->translate(position);
	DirectionalLight *light = (DirectionalLight*)go->getComponent(LIGHT);
	light->directory = directory;
	return addGameObject(move(go));
}

GameObject* TreeScene::createCamera(glm::vec3 position, float yaw, float pitch) {
	unique_ptr<GameObject> go(new GameObject());
	shared_ptr<Camera> camera = shared_ptr<Camera>(new Camera(position, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch));
	go->addComponent(camera);
	shared_ptr<CameraBehaviour> cameraBehaviour = shared_ptr<CameraBehaviour>(new CameraBehaviour(camera.get()));
	go->addComponent(cameraBehaviour);
	return addGameObject(move(go));
}

void TreeScene::generateTestBox(vec3 pos, vec3 scale = vec3(1,1,1)) {
	unique_ptr<GameObject> go = unique_ptr<GameObject>(new Cube(Material::container()));
	go->name = "CrateCube";
	Transform* transform = getTransform(go.get());
	transform->translate(pos);
	shared_ptr<RigidBody> rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1);
	rigidBody->makeDynamic();

	addGameObject(move(go));
}

void TreeScene::addSkybox() {
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

void TreeScene::generateFrameBuffer() {
	setFrameBuffer(unique_ptr<FrameBuffer>(new HDRFrameBuffer()));
}