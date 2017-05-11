#include "Mesh.h"
#include "GLVertexArray.h"

void Mesh::draw(Renderer &renderer) {
	Transform *transform = (Transform*)this->getComponent(TRANSFORM);
	glm::mat4 model = transform->getModelMatrix();

	shader.use();
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	renderer.renderObject(*vertexArray, shader);
}

Mesh::Mesh(vector<Vertex> vertices, vector<int> indices, Shader shader) : vertices(vertices), indices(indices), Renderable(shader) {
	setupMesh();
}

Mesh::~Mesh() {

}

void Mesh::setupMesh() {
	vertexArray = unique_ptr<VertexArray>(new GLVertexArray());
	vertexArray->setVertexArray(vertices, indices);
	shader.registerMatriciesUBO();
}