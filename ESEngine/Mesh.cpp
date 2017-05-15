#include "Mesh.h"
#include "GLVertexArray.h"

void Mesh::draw(Renderer &renderer) {
	Transform *transform = (Transform*)this->getComponent(TRANSFORM);
	GLuint program = shader.program;
	shader.use();

	glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, glm::value_ptr(material.ambient));
	glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, glm::value_ptr(material.diffuse));
	glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, glm::value_ptr(material.specular));
	glUniform1f(glGetUniformLocation(program, "material.shininess"), material.shininess);

	GLuint modelLocation = glGetUniformLocation(program, "model");
	GLuint modelNormalLocation = glGetUniformLocation(program, "normalModel");


	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(transform->getModelMatrix()));
	glUniformMatrix3fv(glGetUniformLocation(program, "normalModel"), 1, GL_FALSE, glm::value_ptr(transform->getNormalModelMatrix()));
	
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
	shader.registerLightsUBO();
}