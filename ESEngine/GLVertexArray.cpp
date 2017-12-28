#include "GLVertexArray.h"

void GLVertexArray::setVertexArray(const std::vector<Vertex> & vertices, const std::vector<int> & indices)
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	//Vertices
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, vBufferSize * sizeof(Vertex),	vertices.size() == 0 ? NULL : &vertices[0], usage);

	//Indices
	indiciesLength = indices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iBufferSize * sizeof(GLuint), indices.size() == 0 ? NULL : &indices[0], usage);
	
	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	
	//Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	
	//Color
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));

	// Vertex Texture Coords
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}

void GLVertexArray::updateVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices) {
	indiciesLength = indices.size();
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), &vertices[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint) * indices.size(), &indices[0]);
}

void GLVertexArray::setInstancedVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices, const std::vector<InstancedTransform> &instanceTransforms) {
	setVertexArray(vertices, indices);

	glBindVertexArray(this->VAO);

	glGenBuffers(1, &instancedModelMatriciesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, instancedModelMatriciesBuffer);
	glBufferData(GL_ARRAY_BUFFER, instanceTransforms.size() * sizeof(InstancedTransform), &instanceTransforms[0], usage);

	//Model matrix
	GLuint vec4Size = sizeof(glm::vec4);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);

	//Normal model matrix

	GLuint vec3Size = sizeof(glm::vec3);
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, 3 * vec3Size, (void*)0);
	glEnableVertexAttribArray(9);
	glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, 3 * vec3Size, (void*)(vec3Size));
	glEnableVertexAttribArray(10);
	glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, 3 * vec3Size, (void*)(2 * vec3Size));

	glVertexAttribDivisor(8, 1);
	glVertexAttribDivisor(9, 1);
	glVertexAttribDivisor(10, 1);

	glBindVertexArray(0);
}

void GLVertexArray::updateInstancedVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices, const std::vector<InstancedTransform> &instanceTransforms) {
	indiciesLength = indices.size();
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), &vertices[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint) * indices.size(), &indices[0]);
	glBindBuffer(GL_ARRAY_BUFFER, instancedModelMatriciesBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, instanceTransforms.size() * sizeof(InstancedTransform), &instanceTransforms[0]);
}
