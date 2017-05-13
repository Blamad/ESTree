#include "Mesh.h"
#include "GLVertexArray.h"

void Mesh::draw(Renderer &renderer) {
	Transform *transform = (Transform*)this->getComponent(TRANSFORM);
	glm::mat4 model = transform->getModelMatrix();
	GLuint program = shader.program;
	shader.use();

	glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, glm::value_ptr(material.ambient));
	glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, glm::value_ptr(material.diffuse));
	glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, glm::value_ptr(material.specular));
	glUniform1f(glGetUniformLocation(program, "material.shininess"), material.shininess);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));

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