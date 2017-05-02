#ifndef MESH_H
#define MESH_H

#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include "Renderable.h"
#include "Renderer.h"
#include "Transform.h"

using namespace std;

class Mesh : public Renderable {
public:
	vector<Vertex> vertices;
	vector<int> indices;

	Mesh(vector<Vertex> vertices, vector<int> indices, Shader shader);
	~Mesh();

	void draw(const Renderer &renderer);
	void draw(const Renderer &renderer, glm::mat4 view, glm::mat4 projection);

	void shout() { std::cout << "Mesh" << std::endl; };

private:
	unique_ptr<VertexArray> vertexArray;
	void setupMesh();
};

#endif