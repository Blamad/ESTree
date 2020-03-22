#include "PhysicsScene.h"

Logger PhysicsScene::logger("PhysicsScene");

void PhysicsScene::initialize() {

	logger.log(LOG_INFO, "Starting scene generation");

	/*int height = 12;
	for (int i = -10; i < 20; i += 10)
		for (int j = -10; j < 20; j += 10)
			generateTower(vec3(i, 0, j), height);*/

	generateTower(glm::vec3(-5, 0, 0), 10);
	generateTower(glm::vec3(5, 0, 0), 10);

	generateTestBox(glm::vec3(0, 2, 12), glm::vec3(1,1,1));

	generateTerrain();
	addSkybox();

	//Light
	glm::vec3 dir = normalize(glm::vec3(-0.5, -0.5, -0.5));
	glm::vec3 distance = glm::vec3(-20);
	createDirectionalLight(distance * dir, dir);

	setActiveCamera(createCamera(glm::vec3(0, 5, 15), -90, -10));

	generateFrameBuffer();

	logger.log(LOG_INFO, "Finished generating scene");
}

void PhysicsScene::generateTower(glm::vec3 center, int floors) {
	float blockHeight = 2 * .3;

	for(int i = 0; i < floors; i++)
		generateFloor(glm::vec3(center.x, i * blockHeight * 2 + center.y, center.z));
}

void PhysicsScene::generateFloor(glm::vec3 center) {
	float blockWidth = 1.5;
	float blockHeight = 2 * .3;
	generateBlock(glm::vec3(-blockWidth + center.x, center.y, center.z), true);
	generateBlock(glm::vec3(blockWidth + center.x, center.y, center.z), true);

	generateBlock(glm::vec3(center.x, center.y + blockHeight, -blockWidth + center.z), false);
	generateBlock(glm::vec3(center.x, center.y + blockHeight, blockWidth + center.z), false);

}

void PhysicsScene::generateBlock(glm::vec3 pos, bool rotate) {
	std::unique_ptr<GameObject> go = std::unique_ptr<GameObject>(new Cube(Material::container(), false, false));
	go->name = "CrateBlock";
	Transform* transform = getTransform(go.get());
	transform->translate(pos);
	transform->scale(glm::vec3(2.5, .3, .3));
	if (rotate)
		transform->rotate(toRadians(90), glm::vec3(0, 1, 0));
	std::shared_ptr<RigidBody> rigidBody = std::shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1);
	rigidBody->makeDynamic();

	addGameObject(move(go));
}

void PhysicsScene::generateTerrain() {
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

void PhysicsScene::generateTestBox(glm::vec3 pos, glm::vec3 scale = glm::vec3(1, 1, 1)) {
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

GameObject* PhysicsScene::createDirectionalLight(glm::vec3 position, glm::vec3 directory) {
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

GameObject* PhysicsScene::createCamera(glm::vec3 position, float yaw, float pitch) {
	std::unique_ptr<GameObject> go(new GameObject());
	std::shared_ptr<Camera> camera = std::shared_ptr<Camera>(new Camera(position, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch));
	go->addComponent(camera);
	std::shared_ptr<CameraBehaviour> cameraBehaviour = std::shared_ptr<CameraBehaviour>(new CameraBehaviour(camera.get()));
	go->addComponent(cameraBehaviour);
	return addGameObject(move(go));
}

void PhysicsScene::addSkybox() {
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

void PhysicsScene::generateFrameBuffer() {
	setFrameBuffer(std::unique_ptr<FrameBuffer>(new HDRFrameBuffer()));
}