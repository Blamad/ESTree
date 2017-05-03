#include "Engine.h"
#include "Window.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Shader.h"
#include "CameraBehaviour.h"
#include "QuadBehaviour.h"

using namespace glm;

GameObject* createCamera(SceneManager *sceneManager);
Vertex createVertex(glm::vec3 position, glm::vec4 color);
GameObject* createSquare(SceneManager *sceneManager);
Engine* initEngine();

int main() {
	Engine* engine = initEngine();
	SceneManager* sceneManager = engine->getSceneManager();
	createCamera(sceneManager);
	createSquare(sceneManager);

	engine->startRendering();

	delete(engine);

	return 0;
}

GameObject* createCamera(SceneManager *sceneManager) {
	return sceneManager->createCamera(vec3(0.0f, 0.0f, 2.0f), -90.0f, 0.0f);
}

GameObject* createSquare(SceneManager *sceneManager) {
	//GameObject *go = sceneManager->createEmptyGameObject();
	GameObject* go = sceneManager->addGameObject(unique_ptr<GameObject>(new GameObject()));

	vector<Vertex> verticies = {
		createVertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
		createVertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
		createVertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),
		createVertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f))
	};
	vector<int> indicies = { 0, 1, 3, 1, 2, 3 };
	Shader shader("GenericShader.vert", "GenericShader.frag");
	shared_ptr<Mesh> mesh = shared_ptr<Mesh>(new Mesh(verticies, indicies, shader));
	go->addComponent(mesh);

	shared_ptr<Transform> transform = shared_ptr<Transform>(new Transform(glm::vec3(0.25f, -0.25f, -2.0f),
		glm::vec4(0.0f, 0.2f, 1.0f, 0.0f),
		glm::vec3(0.7f, 1.0f, 1.2f)));
	go->addComponent(transform);

	shared_ptr<Behaviour> behaviour = shared_ptr<Behaviour>(new QuadBehaviour());
	go->addComponent(behaviour);

	return go;
}

Engine* initEngine() {
	Engine* engine = new Engine();
	Window* window = engine->initialize(800, 600);
	return engine;
}

Vertex createVertex(glm::vec3 position, glm::vec4 color) {
	Vertex vert;
	vert.position = position;
	vert.color = color;
	vert.type[POSITION] = 1;
	vert.type[COLOR] = 1;

	return vert;
}