#include "TestScene.h"

void TestScene::initialize() {

	Transform* transform;
	std::shared_ptr<RigidBody> rigidBody;

	std::unique_ptr<GameObject> go(new Cube(Material::bark1()));
	go->name = "WoodenCube";
	transform = getTransform(go.get());
	transform->translate(glm::vec3(3, 1, 3));
	go->addComponent(std::shared_ptr<Behaviour>(new RotationBehaviour()));
	rigidBody = std::shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(2);
	rigidBody->makeDynamic();
	addGameObject(move(go));

	go = std::unique_ptr<GameObject>(new Cube(Material::container()));
	go->name = "CrateCube";
	transform = getTransform(go.get());
	transform->translate(glm::vec3(0, 6, -2));
	transform->rotate(45.0f, glm::vec3(1, 1, 1));
	rigidBody = std::shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(2);
	rigidBody->makeDynamic();
	addGameObject(move(go));

	go = std::unique_ptr<GameObject>(new Cube(Material::ruby()));
	go->name = "RubyCube";
	transform = getTransform(go.get());
	transform->translate(glm::vec3(0, 10, 0));
	transform->rotate(45.0f, glm::vec3(1, 1, 1));
	rigidBody = std::shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(2);
	rigidBody->makeDynamic();

	addGameObject(move(go));

	go = std::unique_ptr<GameObject>(new Cube(Material::cyanPlastic()));
	go->name = "CyanPlasticCube";
	transform = getTransform(go.get());
	transform->translate(glm::vec3(5, 12, 2));
	transform->rotate(45.0f, glm::vec3(1, 1, 1));
	rigidBody = std::shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(2);
	rigidBody->makeDynamic();

	addGameObject(move(go));

	generateTerrain();
	generateSkybox();
	Context::setHdrToggle(true);

	//Light
	createWhiteLampCube(glm::vec3(-15, 20, 0), WEAK);
	createWhiteLampCube(glm::vec3(15, 20, 0), WEAK);
	createWhiteLampCube(glm::vec3(0, -5, 0), WEAK);
	glm::vec3 dir = normalize(glm::vec3(-1, -0.5, -1));
	createDirectionalLight(glm::vec3(dir.x * -10, 15, dir.z * -10), dir);
	setActiveCamera(createCamera(glm::vec3(0, 5, 15), -90, -10));

	generateFrameBuffer();
}

void TestScene::generateTerrain() {
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
			go->name = "Floor";
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

GameObject* TestScene::createCamera(glm::vec3 position, float yaw, float pitch) {
	std::unique_ptr<GameObject> go(new GameObject());
	std::shared_ptr<Camera> camera = std::shared_ptr<Camera>(new Camera(position, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch));
	go->addComponent(camera);
	std::shared_ptr<CameraBehaviour> cameraBehaviour = std::shared_ptr<CameraBehaviour>(new CameraBehaviour(camera.get()));
	go->addComponent(cameraBehaviour);
	return addGameObject(move(go));
}

GameObject* TestScene::createWhiteLampCube(glm::vec3 position, PointLightStrength str) {
	glm::vec4 color = glm::vec4(1, 1, 1, 1);
	std::unique_ptr<GameObject> go(new LampCube(color));
	go->name = "LampCube";
	go->addComponent(std::shared_ptr<PointLight>(new PointLight(str)));
	Transform* transform = getTransform(go.get());
	transform->translate(position);
	transform->scale(glm::vec3(.4, .4, .4));
	return addGameObject(move(go));
}

GameObject* TestScene::createDirectionalLight(glm::vec3 position, glm::vec3 directory) {
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

GameObject* TestScene::createRotatingCube(Material material) {
	GameObject* go = addGameObject(std::unique_ptr<GameObject>(new Cube(material)));
	go->name = "RotatingCube";
	go->addComponent(std::shared_ptr<Behaviour>(new RotationBehaviour()));
	return go;
}

GameObject* TestScene::createCube(Material material) {
	GameObject* go = addGameObject(std::unique_ptr<GameObject>(new Cube(material)));
	go->name = "Cube";
	return go;
}

void TestScene::generateSkybox() {
	std::string skyboxTex[] = {
		"Textures/Skybox_darkforest/right.jpg",
		"Textures/Skybox_darkforest/left.jpg",
		"Textures/Skybox_darkforest/top.jpg",
		"Textures/Skybox_darkforest/bottom.jpg",
		"Textures/Skybox_darkforest/back.jpg",
		"Textures/Skybox_darkforest/front.jpg"
	};

	std::unique_ptr<Skybox> skybox(new Skybox(skyboxTex));
	setSkybox(std::move(skybox));
}

void TestScene::generateFrameBuffer() {
	setFrameBuffer(std::unique_ptr<FrameBuffer>(new HDRFrameBuffer()));
}