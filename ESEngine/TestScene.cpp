#include "TestScene.h"

void TestScene::initialize() {

	Transform* transform;
	shared_ptr<RigidBody> rigidBody;

	unique_ptr<GameObject> go(new Cube(Material::bark4()));
	go->name = "WoodenCube";
	transform = getTransform(go.get());
	transform->translate(vec3(3, 1, 3));
	go->addComponent(shared_ptr<Behaviour>(new RotationBehaviour()));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(2);
	rigidBody->makeDynamic();
	addGameObject(move(go));

	go = unique_ptr<GameObject>(new Cube(Material::container()));
	go->name = "CrateCube";
	transform = getTransform(go.get());
	transform->translate(vec3(0, 6, -2));
	transform->rotate(45.0f, vec3(1, 1, 1));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(2);
	rigidBody->makeDynamic();
	addGameObject(move(go));

	go = unique_ptr<GameObject>(new Cube(Material::container()));
	go->name = "CrateCube";
	transform = getTransform(go.get());
	transform->translate(vec3(0, 10, 0));
	transform->rotate(45.0f, vec3(1, 1, 1));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(2);
	rigidBody->makeDynamic();

	addGameObject(move(go));

	go = unique_ptr<GameObject>(new Cube(Material::ruby()));
	go->name = "RedCube";
	transform = getTransform(go.get());
	transform->translate(vec3(5, 12, 2));
	transform->rotate(45.0f, vec3(1, 1, 1));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(2);
	rigidBody->makeDynamic();

	addGameObject(move(go));

	go = unique_ptr<GameObject>(new Cube(Material::container()));
	go->name = "CrateCube";
	transform = getTransform(go.get());
	transform->translate(vec3(1, 6, 1));
	transform->rotate(45.0f, vec3(1, 1, 1));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(2);
	rigidBody->makeDynamic();

	addGameObject(move(go));

	generateTerrain();
	generateSkybox();

	//Light
	createWhiteLampCube(vec3(-15, 20, 0), WEAK);
	//createWhiteLampCube(vec3(15, 20, 0), WEAK);
	//createWhiteLampCube(vec3(0, -5, 0), WEAK);
	createDirectionalLight(vec3(-1, -0.5, -1));

	generateFrameBuffer();
}

void TestScene::generateTerrain() {
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
			go->name = "Floor";
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

GameObject* TestScene::createWhiteLampCube(vec3 position, PointLightStrength str) {
	vec4 color = vec4(1, 1, 1, 1);
	unique_ptr<GameObject> go(new LampCube(color));
	go->name = "LampCube";
	go->addComponent(shared_ptr<PointLight>(new PointLight(str)));
	Transform* transform = getTransform(go.get());
	transform->translate(position);
	transform->scale(vec3(.4, .4, .4));
	return addGameObject(move(go));
}

GameObject* TestScene::createDirectionalLight(vec3 directory) {
	unique_ptr<GameObject> go(new GameObject());
	go->addComponent(shared_ptr<DirectionalLight>(new DirectionalLight()));
	go->name = "DirLight";
	DirectionalLight *light = (DirectionalLight*)go->getComponent(LIGHT);
	light->directory = directory;
	return addGameObject(move(go));
}

GameObject* TestScene::createRotatingCube(Material material) {
	GameObject* go = addGameObject(unique_ptr<GameObject>(new Cube(material)));
	go->name = "RotatingCube";
	go->addComponent(shared_ptr<Behaviour>(new RotationBehaviour()));
	return go;
}

GameObject* TestScene::createCube(Material material) {
	GameObject* go = addGameObject(unique_ptr<GameObject>(new Cube(material)));
	go->name = "Cube";
	return go;
}

void TestScene::generateSkybox() {
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

void TestScene::generateFrameBuffer() {
	setFrameBuffer(unique_ptr<FrameBuffer>(new HDRFrameBuffer()));
}