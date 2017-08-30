#include "Mesh.h"

void Mesh::draw(Renderer &renderer) {
	Transform *transform = (Transform*)this->getComponent(TRANSFORM);
	GLuint program = shader.program;
	shader.use();

	if (!initialized) {
		glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, glm::value_ptr(material.ambient));
		glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, glm::value_ptr(material.diffuse));
		glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, glm::value_ptr(material.specular));
		glUniform1f(glGetUniformLocation(program, "material.shininess"), material.shininess);
		glUniform1f(glGetUniformLocation(program, "material.texDiffuse"), 0);
		glUniform1f(glGetUniformLocation(program, "material.texSpecular"), 1);
		initialized = true;
	}

	if (material.texDiffuse != nullptr) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.texDiffuse->textureBuffer->id);
	}

	if (material.texSpecular != nullptr) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.texSpecular->textureBuffer->id);
	}

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(transform->getModelMatrix()));
	glUniformMatrix3fv(glGetUniformLocation(program, "normalModel"), 1, GL_FALSE, glm::value_ptr(transform->getNormalModelMatrix()));
	
	renderer.renderObject(*vertexArray, shader);

	if (material.texDiffuse != nullptr) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (material.texSpecular != nullptr) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

Mesh::Mesh(vector<Vertex> &vertices, vector<int> &indices, Shader &shader, int vBufferSize, int iBufferSize, int bufferUsage) : vertices(vertices), indices(indices), Renderable(shader) {
	if (vBufferSize == -1)
		vBufferSize = vertices.size();
	if (iBufferSize == -1)
		iBufferSize = indices.size();

	vertexArray = unique_ptr<VertexArray>(new GLVertexArray(vBufferSize, iBufferSize, bufferUsage));
	this->bufferUsage = bufferUsage;
	setupMesh();
}

Mesh::~Mesh() {

}

void Mesh::setupMesh() {
	vertexArray->setVertexArray(vertices, indices);
	shader.registerMatriciesUBO();
	shader.registerLightsUBO();
}

void Mesh::updateMesh() {
	vertexArray->updateVertexArray(vertices, indices);
}