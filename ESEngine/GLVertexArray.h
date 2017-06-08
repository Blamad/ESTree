#ifndef GLVERTEXARRAY_H
#define GLVERTEXARRAY_H

#include <GL/glew.h>
#include "VertexArray.h"

class GLVertexArray : public VertexArray
{
public:
	GLuint VAO, VBO, EBO, indiciesLength;
	
	GLVertexArray(int vBufferSize = -1, int iBufferSize = -1, int usage = GL_STATIC_DRAW) : vBufferSize(vBufferSize), iBufferSize(iBufferSize), usage(usage) {};
	void setVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices) override;
	void updateVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices) override;

private:
	GLuint vBufferSize;
	GLuint iBufferSize;
	GLuint usage;
};

#endif