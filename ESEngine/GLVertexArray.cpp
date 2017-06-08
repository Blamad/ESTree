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
	
	// Vertex Texture Coords
	/*if (flags[TEXCOORDS])
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index++, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));
	} else*/
	
	//Color
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));

	glBindVertexArray(0);
}

void GLVertexArray::updateVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices) {
	indiciesLength = indices.size();
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), &vertices[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint) * indices.size(), &indices[0]);
}
