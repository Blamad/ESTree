#include "Engine.h"
#include "Window.h"
#include "Tree.h"
#include "LindenmayerTree.h"
#include "LampCube.h"
#include "Cube.h"
#include "InstancedCube.h"
#include "RotationBehaviour.h"
#include "RigidBody.h"

using namespace glm;

GameObject* createCamera(SceneManager *sceneManager, vec3 position, float pitch, float yaw);
GameObject* createLindenmayerTree(SceneManager *sceneManager, LindenmayerTreeParams &params, vec3 &position, Material &material = Material::copper());
GameObject* createLindenmayerTree(SceneManager *sceneManager, string configurationFileName, vec3 &position, Material &material, Material &leavesMaterial);

GameObject* createCube(SceneManager *sceneManager, Material material);
GameObject* createRotatingCube(SceneManager *sceneManager, Material material);

GameObject* createWhiteLampCube(SceneManager *sceneManager, vec3 position, PointLightStrength str);
GameObject* createWhiteLampCube(SceneManager *sceneManager, vec3 position, vec3 ambient, vec3 specular, vec3 diffuse, float constant, float linear, float quadratic);
GameObject* createDirectionalLight(SceneManager *sceneManager, vec3 direction);

Transform* getTransform(GameObject *gameObject);
void testScene(SceneManager *sceneManager);
void treeScene(SceneManager *sceneManager);
void generateTerrain(SceneManager *sceneManager);

Engine* initEngine();

int main() {
	Engine* engine = initEngine();
	SceneManager* sceneManager = engine->getSceneManager();

	treeScene(sceneManager);
	//testScene(sceneManager);

	engine->startRendering();

	delete(engine);

	return 0;
}

void treeScene(SceneManager *sceneManager) {
	createCamera(sceneManager, vec3(0, 15, -45), 90, -10);
	GameObject* go;
	vec3 position;
	string paramsFileName;

	paramsFileName = "symetricTree.l";
	position = vec3(-15, 0, -7);
	go = createLindenmayerTree(sceneManager, paramsFileName, position, Material::bark4(), Material::leaves3());

	paramsFileName = "advancedTree.l";
	position = vec3(0, 0, 0);
	go = createLindenmayerTree(sceneManager, paramsFileName, position, Material::bark2(), Material::leaves1());

	paramsFileName = "randomTree.l";
	position = vec3(15, 0, -7);
	go = createLindenmayerTree(sceneManager, paramsFileName, position, Material::bark5(), Material::leaves3());

	generateTerrain(sceneManager);

	//Light
	createWhiteLampCube(sceneManager, vec3(-15, 20, 0), MEDIUM);
	createWhiteLampCube(sceneManager, vec3(15, 20, 0), MEDIUM);
	createWhiteLampCube(sceneManager, vec3(0, -5, 0), WEAK);
	createDirectionalLight(sceneManager, vec3(0, -1, -1));

}

GameObject* createLindenmayerTree(SceneManager *sceneManager, LindenmayerTreeParams &params, vec3 &position, Material &material) {
	GameObject* go = sceneManager->addGameObject(unique_ptr<GameObject>(new LindenmayerTree(params, material)));
	Transform *transform = getTransform(go);
	transform->translate(position);
	((LindenmayerTree*)go)->generate();
	return go;
}

GameObject* createLindenmayerTree(SceneManager *sceneManager, string paramsFileName, vec3 &position, Material &material, Material &leavesMaterial) {
	LindenmayerTreeParams params = LindenmayerTreeParams(paramsFileName);
	GameObject* go = sceneManager->addGameObject(unique_ptr<GameObject>(new LindenmayerTree(params, material, leavesMaterial)));
	Transform *transform = getTransform(go);
	transform->translate(position);
	((LindenmayerTree*)go)->generate();
	return go;
}

void testScene(SceneManager *sceneManager) {
	createCamera(sceneManager, vec3(0, 5, -12), 90, -10);
	Transform* transform;
	shared_ptr<RigidBody> rigidBody;

	/*unique_ptr<GameObject> go(new Cube(Material::stem1()));
	transform = getTransform(go.get());
	transform->translate(vec3(0, 1, 0));
	go->addComponent(shared_ptr<Behaviour>(new RotationBehaviour()));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(0, vec3(1, 1, 1));
	sceneManager->addGameObject(move(go));

	go = unique_ptr<GameObject>(new Cube(Material::container()));
	transform = getTransform(go.get());
	transform->translate(vec3(0, 6, -2));
	transform->rotate(45.0f, vec3(1, 1, 1));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1, vec3(1, 1, 1));
	sceneManager->addGameObject(move(go));

	go = unique_ptr<GameObject>(new Cube(Material::container()));
	transform = getTransform(go.get());
	transform->translate(vec3(0, 10, 0));
	transform->rotate(45.0f, vec3(1, 1, 1));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1, vec3(1, 1, 1));
	sceneManager->addGameObject(move(go));

	go = unique_ptr<GameObject>(new Cube(Material::ruby()));
	transform = getTransform(go.get());
	transform->translate(vec3(5, 12, 2));
	transform->rotate(45.0f, vec3(1, 1, 1));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1, vec3(1, 1, 1));
	sceneManager->addGameObject(move(go));

	go = unique_ptr<GameObject>(new Cube(Material::container()));
	transform = getTransform(go.get());
	transform->translate(vec3(1, 6, 1));
	transform->rotate(45.0f, vec3(1, 1, 1));
	rigidBody = shared_ptr<RigidBody>(new RigidBody());
	go->addComponent(rigidBody);
	rigidBody->initAsBox(1, vec3(1, 1, 1));
	sceneManager->addGameObject(move(go));*/
	vector<InstancedTransform> transforms;
	InstancedTransform it;
	it.translateModel(vec3(0, 1, 0));
	it.generateNormalModelMatrix();
	transforms.push_back(it);

	unique_ptr<InstancedCube> ic(new InstancedCube(Material::leaves2(), transforms));
	sceneManager->addGameObject(move(ic));

	unique_ptr<GameObject> go(new Cube(Material::leaves2()));
	transform = getTransform(go.get());
	transform->translate(vec3(0, 1, 0));
	sceneManager->addGameObject(move(go));

	//generateTerrain(sceneManager);

	//Light
	int distance = 15;
	int height = 15;

	createWhiteLampCube(sceneManager, vec3(distance, height, distance), vec3(0.05f, 0.05f, 0.05f), vec3(1,1,1), vec3(1,1,1), 1, 0.022, 0.0019);
	createWhiteLampCube(sceneManager, vec3(-distance, height, distance), vec3(0.05f, 0.05f, 0.05f), vec3(1,1,1), vec3(1,1,1), 1, 0.022, 0.0019);
	createWhiteLampCube(sceneManager, vec3(distance, height, -distance), vec3(0.05f, 0.05f, 0.05f), vec3(1,1,1), vec3(1,1,1), 1, 0.022, 0.0019);
	createWhiteLampCube(sceneManager, vec3(-distance, height, -distance), vec3(0.05f, 0.05f, 0.05f), vec3(1,1,1), vec3(1,1,1), 1, 0.022, 0.0019);
	createDirectionalLight(sceneManager, vec3(0, -1, 0));
}

void generateTerrain(SceneManager* sceneManager) {
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
			sceneManager->addGameObject(move(go));
		}
	}
}

GameObject* createDirectionalLight(SceneManager *sceneManager, vec3 directory) {
	unique_ptr<GameObject> go(new GameObject());
	go->addComponent(shared_ptr<DirectionalLight>(new DirectionalLight()));
	DirectionalLight *light = (DirectionalLight*)go->getComponent(LIGHT);
	light->directory = directory;
	return sceneManager->addGameObject(move(go));
}

Transform* getTransform(GameObject* gameObject) {
	return (Transform*)gameObject->getComponent(TRANSFORM);
}

GameObject* createCamera(SceneManager *sceneManager, vec3 position, float pitch, float yaw) {
	return sceneManager->createCamera(position, pitch, yaw);
}

GameObject* createCube(SceneManager *sceneManager, Material material) {
	GameObject* go = sceneManager->addGameObject(unique_ptr<GameObject>(new Cube(material)));
	return go;
}

GameObject* createRotatingCube(SceneManager *sceneManager, Material material) {
	GameObject* go = sceneManager->addGameObject(unique_ptr<GameObject>(new Cube(material)));
	go->addComponent(shared_ptr<Behaviour>(new RotationBehaviour()));
	return go;
}

GameObject* createWhiteLampCube(SceneManager *sceneManager, vec3 position, PointLightStrength str) {
	vec4 color = vec4(1, 1, 1, 1);
	unique_ptr<GameObject> go(new LampCube(color));
	go->addComponent(shared_ptr<PointLight>(new PointLight(str)));
	Transform* transform = getTransform(go.get());
	transform->translate(position);
	transform->scale(vec3(.4, .4, .4));
	return sceneManager->addGameObject(move(go));
}

GameObject* createWhiteLampCube(SceneManager *sceneManager, vec3 position, vec3 ambient, vec3 specular, vec3 diffuse, float constant, float linear, float quadratic) {
	vec4 color = vec4(1, 1, 1, 1);
	unique_ptr<GameObject> go(new LampCube(color));
	go->addComponent(shared_ptr<PointLight>(new PointLight(constant, linear, quadratic, ambient, diffuse, specular)));
	Transform* transform = getTransform(go.get());
	transform->translate(position);
	transform->scale(vec3(.4, .4, .4));
	
	return sceneManager->addGameObject(move(go));
}

Engine* initEngine() {
	Engine* engine = new Engine();
	Window* window = engine->initialize(1024, 768);
	return engine;
}