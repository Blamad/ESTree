#include "TestScene.h"
#include "Cube.h"
#include "LampCube.h"
#include "InstancedCube.h"
#include "RotationBehaviour.h"
#include "RigidBody.h"

void TestScene::initialize() {

	Transform* transform;
	shared_ptr<RigidBody> rigidBody;

	unique_ptr<GameObject> go(new Cube(Material::bark4()));
	transform = getTransform(go.get());
	transform->translate(vec3(3, 1, 3));
	go->addComponent(shared_ptr<Behaviour>(new RotationBehaviour()));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(0, vec3(1, 1, 1));
	addGameObject(move(go));

	go = unique_ptr<GameObject>(new Cube(Material::container()));
	transform = getTransform(go.get());
	transform->translate(vec3(0, 6, -2));
	transform->rotate(45.0f, vec3(1, 1, 1));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1, vec3(1, 1, 1));
	addGameObject(move(go));

	go = unique_ptr<GameObject>(new Cube(Material::container()));
	transform = getTransform(go.get());
	transform->translate(vec3(0, 10, 0));
	transform->rotate(45.0f, vec3(1, 1, 1));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1, vec3(1, 1, 1));
	addGameObject(move(go));

	go = unique_ptr<GameObject>(new Cube(Material::ruby()));
	transform = getTransform(go.get());
	transform->translate(vec3(5, 12, 2));
	transform->rotate(45.0f, vec3(1, 1, 1));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1, vec3(1, 1, 1));
	addGameObject(move(go));

	go = unique_ptr<GameObject>(new Cube(Material::container()));
	transform = getTransform(go.get());
	transform->translate(vec3(1, 6, 1));
	transform->rotate(45.0f, vec3(1, 1, 1));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1, vec3(1, 1, 1));
	addGameObject(move(go));

	vector<InstancedTransform> transforms;
	InstancedTransform it;
	it.translateModel(vec3(0, 1, 0));
	it.generateNormalModelMatrix();
	transforms.push_back(it);

	unique_ptr<InstancedCube> ic(new InstancedCube(Material::leaves2(), transforms));
	addGameObject(move(ic));

	generateTerrain();

	//Light
	createWhiteLampCube(vec3(-15, 20, 0), MEDIUM);
	createWhiteLampCube(vec3(15, 20, 0), MEDIUM);
	createWhiteLampCube(vec3(0, -5, 0), WEAK);
	createDirectionalLight(vec3(0, -1, -1));
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

GameObject* TestScene::createWhiteLampCube(vec3 position, PointLightStrength str) {
	vec4 color = vec4(1, 1, 1, 1);
	unique_ptr<GameObject> go(new LampCube(color));
	go->addComponent(shared_ptr<PointLight>(new PointLight(str)));
	Transform* transform = getTransform(go.get());
	transform->translate(position);
	transform->scale(vec3(.4, .4, .4));
	return addGameObject(move(go));
}

GameObject* TestScene::createDirectionalLight(vec3 directory) {
	unique_ptr<GameObject> go(new GameObject());
	go->addComponent(shared_ptr<DirectionalLight>(new DirectionalLight()));
	DirectionalLight *light = (DirectionalLight*)go->getComponent(LIGHT);
	light->directory = directory;
	return addGameObject(move(go));
}

GameObject* TestScene::createRotatingCube(Material material) {
	GameObject* go = addGameObject(unique_ptr<GameObject>(new Cube(material)));
	go->addComponent(shared_ptr<Behaviour>(new RotationBehaviour()));
	return go;
}

GameObject* TestScene::createCube(Material material) {
	GameObject* go = addGameObject(unique_ptr<GameObject>(new Cube(material)));
	return go;
}