#include "Engine.h"
#include "Window.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Shader.h"
#include "CameraBehaviour.h"

void createCamera(Scene* scene);
void createSquare(Scene* scene);
Engine* initEngine();

int main() {
	Engine* engine = initEngine();
	Scene* scene = engine->getSceneManager()->getActiveScene();
	createCamera(scene);
	createSquare(scene);

	engine->startRendering();

	return 0;
}

Engine* initEngine() {
	Engine* engine = new Engine();
	Window* window = engine->initialize(800, 600);
	return engine;
}

void createCamera(Scene *scene) {
	GameObject *go = scene->createGameObject();

	shared_ptr<Camera> camera = shared_ptr<Camera>(new Camera(glm::vec3(0.0f, 0.0f, -3.0f)));
	go->addComponent(camera);

	shared_ptr<CameraBehaviour> cameraBehaviour = shared_ptr<CameraBehaviour>(new CameraBehaviour(camera.get()));
	go->addComponent(cameraBehaviour);

	scene->setActiveCamera(go);
}

Vertex createVertex(glm::vec3 position, glm::vec4 color) {
	Vertex vert;
	vert.position = position;
	vert.color = color;
	vert.type[POSITION] = 1;
	vert.type[COLOR] = 1;

	return vert;
}

void createSquare(Scene* scene) {
	GameObject *go = scene->createGameObject();

	vector<Vertex> verticies = {
		createVertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
		createVertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
		createVertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),
		createVertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f))
	};
	vector<int> indicies = {0, 1, 3, 1, 2, 3};
	Shader shader("GenericShader.vert", "GenericShader.frag");
	shared_ptr<Mesh> mesh = shared_ptr<Mesh>(new Mesh(verticies, indicies, shader));
	go->addComponent(mesh);

	shared_ptr<Transform> transform = shared_ptr<Transform>(new Transform(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)));
	go->addComponent(transform);
}