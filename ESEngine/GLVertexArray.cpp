#include "GLVertexArray.h"

void GLVertexArray::setVertexArray(const std::vector<Vertex> & vertices, const std::vector<int> & indices)
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),	&vertices[0], GL_STATIC_DRAW);

	if (!indices.empty())
	{
		indiciesNum = indices.size();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	}

	std::bitset<4U> flags = vertices[0].type;
	// Vertex Positions
	if (flags[POSITION])
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	}
	// Vertex Normals
	if (flags[NORMAL])
	{
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	}
	// Vertex Texture Coords
	/*if (flags[TEXCOORDS])
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index++, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));
	} else*/
	if (flags[COLOR])
	{
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	}

	glBindVertexArray(0);
}
