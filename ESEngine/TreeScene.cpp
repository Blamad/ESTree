#include "TreeScene.h"

Logger TreeScene::logger("TreeScene");

void TreeScene::initialize() {

	logger.log(INFO, "Starting scene");

	GameObject* go;
	vec3 position;
	string paramsFileName;
	
	//paramsFileName = "LindenmayerRules/parametricMonopodialTreeA.l";
	paramsFileName = "LindenmayerRules/parametricTernaryTreeA.l";
	position = vec3(10, 0, 10);
	go = createLindenmayerTree(paramsFileName, position, Material::bark1(), Material::diffuseTextureOnly("Textures/leaves1.png"), true);

	/*paramsFileName = "LindenmayerRules/parametricSympodialTreeB.l";
	position = vec3(-10, 0, 10);
	go = createLindenmayerTree(paramsFileName, position, Material::bark2(), Material::diffuseTextureOnly("Textures/leaves2.png"), true);

	paramsFileName = "LindenmayerRules/parametricSympodialTreeC.l";
	position = vec3(10, 0, -10);
	go = createLindenmayerTree(paramsFileName, position, Material::bark3(), Material::diffuseTextureOnly("Textures/leaves3.png"), true);

	paramsFileName = "LindenmayerRules/parametricSympodialTreeD.l";
	position = vec3(-10, 0, -10);
	go = createLindenmayerTree(paramsFileName, position, Material::bark4(), Material::diffuseTextureOnly("Textures/leaves4.png"), true);

	/*paramsFileName = "LindenmayerRules/parametricMonopodialTreeB.l";
	position = vec3(-10, 0, 10);
	go = createLindenmayerTree(paramsFileName, position, Material::bark2(), Material::diffuseTextureOnly("Textures/leaves2.png"), true);

	paramsFileName = "LindenmayerRules/parametricMonopodialTreeC.l";
	position = vec3(10, 0, -10);
	go = createLindenmayerTree(paramsFileName, position, Material::bark3(), Material::diffuseTextureOnly("Textures/leaves3.png"), true);

	paramsFileName = "LindenmayerRules/parametricMonopodialTreeD.l";
	position = vec3(-10, 0, -10);
	go = createLindenmayerTree(paramsFileName, position, Material::bark4(), Material::diffuseTextureOnly("Textures/leaves4.png"), true);

	/*
	paramsFileName = "LindenmayerRules/randomTree.l";
	position = vec3(10, 0, 10);
	go = createLindenmayerTree(paramsFileName, position, Material::bark1(), Material::diffuseTextureOnly("Textures/leaves5.png"));

	paramsFileName = "LindenmayerRules/randomTree.l";
	position = vec3(10, 0, -10);
	go = createLindenmayerTree(paramsFileName, position, Material::bark5(), Material::leaves2());
	
	paramsFileName = "LindenmayerRules/symetricTree.l";
	position = vec3(-10, 0, 10);
	go = createLindenmayerTree(paramsFileName, position, Material::bark5(), Material::diffuseTextureOnly("Textures/leaves4.png"));

	
	paramsFileName = "LindenmayerRules/fibbonacciTree.l";
	position = vec3(20, 0, 0);
	go = createLindenmayerTree(paramsFileName, position, Material::bark5(), Material::leaves1());

	paramsFileName = "LindenmayerRules/advancedTree2.l";
	position = vec3(8, 0, 0);
	go = createLindenmayerTree(paramsFileName, position, Material::bark3(), Material::diffuseTextureOnly("Textures/leaves3.png"));

	paramsFileName = "LindenmayerRules/advancedTree2.l";
	position = vec3(-5, 0, 0);
	go = createLindenmayerTree(paramsFileName, position, Material::bark4(), Material::diffuseTextureOnly("Textures/leaves5.png"));

	paramsFileName = "LindenmayerRules/advancedTree2.l";
	position = vec3(0, 0, 15);
	go = createLindenmayerTree(paramsFileName, position, Material::bark3(), Material::diffuseTextureOnly("Textures/leaves1.png"));

	paramsFileName = "LindenmayerRules/advancedTree2.l";
	position = vec3(0, 0, -15);
	go = createLindenmayerTree(paramsFileName, position, Material::bark1(), Material::diffuseTextureOnly("Textures/leaves3.png"));*/

	generateTerrain();
	addSkybox();

	//Light
	createWhiteLampCube(vec3(-10, 30, 0), MEDIUM);
	createWhiteLampCube(vec3(10, 30, 0), MEDIUM);
	createWhiteLampCube(vec3(0, -5, 0), WEAK);
	createDirectionalLight(vec3(0, -1, -1));

	setActiveCamera(createCamera(vec3(-20, 15, -35), 45, -10));
}

GameObject* TreeScene::createLindenmayerTree(string paramsFileName, vec3 &position, Material &material, Material &leavesMaterial, bool debug) {
	LindenmayerTreeParams params = LindenmayerTreeParams(paramsFileName);
	GameObject* go = addGameObject(unique_ptr<GameObject>(new LindenmayerTree(params, material, leavesMaterial, debug)));
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

GameObject* TreeScene::createCamera(glm::vec3 position, float yaw, float pitch) {
	unique_ptr<GameObject> go(new GameObject());
	shared_ptr<Camera> camera = shared_ptr<Camera>(new Camera(position, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch));
	go->addComponent(camera);
	shared_ptr<CameraBehaviour> cameraBehaviour = shared_ptr<CameraBehaviour>(new CameraBehaviour(camera.get()));
	go->addComponent(cameraBehaviour);
	return addGameObject(move(go));
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