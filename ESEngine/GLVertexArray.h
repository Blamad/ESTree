#ifndef GLVERTEXARRAY_H
#define GLVERTEXARRAY_H

#include <GL/glew.h>
#include "VertexArray.h"

class GLVertexArray : public VertexArray
{
public:
	GLuint VAO, VBO, EBO, indiciesNum;
	
	GLVertexArray() {};
	void setVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices) override;
};

#endif