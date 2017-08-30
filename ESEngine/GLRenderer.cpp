#include "GLRenderer.h"
#include <iostream>

void GLRenderer::renderObject(VertexArray &vertexArray, Shader &shader) {

	const GLVertexArray &vArray = static_cast <const GLVertexArray&>(vertexArray);

	glBindVertexArray(vArray.VAO);
	glDrawElements(GL_TRIANGLES, vArray.indiciesLength, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void GLRenderer::renderInstancedObject(VertexArray &vertexArray, Shader &shader, int &amount) {

	const GLVertexArray &vArray = static_cast <const GLVertexArray&>(vertexArray);

	glBindVertexArray(vArray.VAO);
	glDrawElementsInstanced(GL_TRIANGLES, vArray.indiciesLength, GL_UNSIGNED_INT, 0, amount);
	glBindVertexArray(0);
}
