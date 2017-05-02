#include "Mesh.h"
#include "GLVertexArray.h"

void Mesh::draw(const Renderer &renderer) {
	Transform *transform = (Transform*)this->getComponent(TRANSFORM);
	
}

void Mesh::draw(const Renderer &renderer, glm::mat4 view, glm::mat4 projection) {
	//TODO tymczasowe rozwiazanie, do usuniecia pozniej!
	GLVertexArray *va = (GLVertexArray*)vertexArray.get();
	Transform *transform = (Transform*)this->getComponent(TRANSFORM);

	glm::mat4 model = transform->getModelMatrix();

	shader.Use();
	glBindVertexArray(va->VAO);
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glDrawElements(GL_TRIANGLES, va->indiciesNum, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Mesh::Mesh(vector<Vertex> vertices, vector<int> indices, Shader shader) : vertices(vertices), indices(indices), Renderable(shader)
{
	setupMesh();
}

Mesh::~Mesh() {

}

void Mesh::setupMesh()
{
	vertexArray = unique_ptr<VertexArray>(new GLVertexArray());
	vertexArray->setVertexArray(vertices, indices);
}