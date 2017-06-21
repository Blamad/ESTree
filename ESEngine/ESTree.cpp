#include "Engine.h"
#include "Window.h"
#include "Tree.h"
#include "LindenmayerTree.h"
#include "LampCube.h"
#include "Cube.h"
#include "RotationBehaviour.h"

using namespace glm;

GameObject* createCamera(SceneManager *sceneManager, vec3 position, float pitch, float yaw);
GameObject* createLindenmayerTree(SceneManager *sceneManager, LindenmayerTreeParams &params, vec3 &position);

GameObject* createCube(SceneManager *sceneManager, Material material);
GameObject* createRotatingCube(SceneManager *sceneManager, Material material);

GameObject* createWhiteLampCube(SceneManager *sceneManager, vec3 position);
GameObject* createDirectionalLight(SceneManager *sceneManager, vec3 direction);

Transform* getTransform(GameObject *gameObject);
void createLights(SceneManager *sceneManager);
void testScene(SceneManager *sceneManager);
void treeScene(SceneManager *sceneManager);

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

	//2D

	LindenmayerTreeParams params = LindenmayerTreeParams("f", 4, 0.5f, 0.2f, 15.0f);
	params.addRule(Rule("f", "f[+f]f[-f]f", .33));
	params.addRule(Rule("f", "f[+f]f", .33));
	params.addRule(Rule("f", "f[-f]f", .34));
	vec3 position = vec3(-10, 0, -20);
	go = createLindenmayerTree(sceneManager, params, position);

	position = vec3(0, 0, -20);
	go = createLindenmayerTree(sceneManager, params, position);

	params = LindenmayerTreeParams("f", 3, 0.5f, 0.2f, 15.0f);
	params.addRule(Rule("f", "ff + [+f - f - f] - [-f + f + f]"));
	position = vec3(10, 0, -20);
	go = createLindenmayerTree(sceneManager, params, position);

	//3D
	
	params = LindenmayerTreeParams("fffa", 4, 1.5f, 0.4f, 15.0f);
	params.addRule(Rule("a", "\"[&&fffa][//fffa][\\\\fffa][^^fffa]"));
	position = vec3(-15, 0, -10); 
	go = createLindenmayerTree(sceneManager, params, position);

	params = LindenmayerTreeParams("F", 4, 0.8, 0.35f, 7.0f);
	params.addRule(Rule("F", "\"Y[++++++MF][-----NF][^^^^^OF][&&&&&PF]"));
	params.addRule(Rule("M", "Z-M"));
	params.addRule(Rule("N", "Z+N"));
	params.addRule(Rule("O", "Z&O"));
	params.addRule(Rule("P", "Z^P"));
	params.addRule(Rule("Y", "Z-ZY+"));
	params.addRule(Rule("Z", "ZZ"));
	position = vec3(-5, 0, -10);
	go = createLindenmayerTree(sceneManager, params, position);

	params = LindenmayerTreeParams("fffa", 4, 1.5f, 0.4f, 15.0f);
	params.addRule(Rule("a", "\"[&&fffa][//fffa][\\\\fffa][^^fffa]", .33));
	params.addRule(Rule("a", "\"[\\\\fffa][//ffa]", .33));
	params.addRule(Rule("a", "\"[&&ffa][^^fffa]", .34));
	position = vec3(5, 0, -10);
	go = createLindenmayerTree(sceneManager, params, position);

	params = LindenmayerTreeParams("f", 3, 0.5f, 0.2f, 15.0f);
	params.addRule(Rule("f", "ff + [+f - f - f] - [-f + f + f]", .5));
	params.addRule(Rule("f", "ff^ [^f &f &f] &[&f ^f ^f]", .5));
	position = vec3(15, 0, -10);
	go = createLindenmayerTree(sceneManager, params, position);

	createWhiteLampCube(sceneManager, vec3(0, 20, -15));
	createDirectionalLight(sceneManager, vec3(0, -1, -1));

	go = createCube(sceneManager, Material::greenRubber());
	Transform *transform = getTransform(go);
	transform->translate(vec3(0, 0, -10));
	transform->scale(vec3(30, 0.3, 30));
}

GameObject* createLindenmayerTree(SceneManager *sceneManager, LindenmayerTreeParams &params, vec3 &position) {
	GameObject* go = sceneManager->addGameObject(unique_ptr<GameObject>(new LindenmayerTree(params)));
	Transform *transform = getTransform(go);
	transform->translate(position);
	return go;
}

void testScene(SceneManager *sceneManager) {
	createCamera(sceneManager, vec3(8, 3, 8), -140, -10);
	Transform* transform;
	GameObject* go;

	go = createRotatingCube(sceneManager, Material::brass());
	transform = getTransform(go);
	transform->translate(vec3(-8, 2, 0));

	go = createRotatingCube(sceneManager, Material::ruby());
	transform = getTransform(go);
	transform->translate(vec3(-12, 2, 0));

	/*go = createCube(sceneManager, Material::obsidian());
	transform = getTransform(go);
	transform->translate(vec3(0, 0, 0));
	transform->scale(vec3(20, 0.1, 20));*/

	createLights(sceneManager);
}

void createLights(SceneManager *sceneManager) {
	createWhiteLampCube(sceneManager, vec3(0, 4.0, .0));
	createWhiteLampCube(sceneManager, vec3(0, -2.0, .0));
	createWhiteLampCube(sceneManager, vec3(-2.0, 1.0, 4.0));
	createWhiteLampCube(sceneManager, vec3(14.0, 4.0, -14.0));
	createDirectionalLight(sceneManager, vec3(-0.2f, -1.0f, -0.3f));
}

GameObject* createDirectionalLight(SceneManager *sceneManager, vec3 directory) {
	GameObject* dirLightObject = sceneManager->addGameObject(unique_ptr<GameObject>(new GameObject()));
	dirLightObject->addComponent(sceneManager->createDirectionalLight());
	DirectionalLight *light = (DirectionalLight*)dirLightObject->getComponent(LIGHT);
	light->directory = directory;

	return dirLightObject;
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

GameObject* createWhiteLampCube(SceneManager *sceneManager,  vec3 position) {
	vec4 color = vec4(1, 1, 1, 1);
	GameObject* go = sceneManager->addGameObject(unique_ptr<GameObject>(new LampCube(color)));
	go->addComponent(sceneManager->createPointLight());
	Transform* transform = getTransform(go);
	transform->translate(position);
	transform->scale(vec3(.2, .2, .2));
	return go;
}

Engine* initEngine() {
	Engine* engine = new Engine();
	Window* window = engine->initialize(1024, 768);
	return engine;
}