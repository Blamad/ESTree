#include "GLRenderer.h"

void GLRenderer::renderObject(const VertexArray &vertexArray, Shader &shader) {

	const GLVertexArray &vArray = static_cast <const GLVertexArray&>(vertexArray);

	shader.Use();
	glBindVertexArray(vArray.VAO);
	glDrawElements(GL_TRIANGLES, vArray.indiciesNum, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}