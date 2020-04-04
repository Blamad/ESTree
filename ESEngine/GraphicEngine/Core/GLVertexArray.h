#ifndef GL_VERTEXARRAY_H
#define GL_VERTEXARRAY_H

#include <glad/glad.h>
#include "VertexArray.h"

class GLVertexArray : public VertexArray
{
public:
	GLuint VAO, VBO, EBO, indiciesLength;
	
	GLVertexArray(int iBufferSize, int vBufferSize, int usage) : iBufferSize(iBufferSize), vBufferSize(vBufferSize), usage(usage) {};
	void setVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices) override;
	void updateVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices) override;
	void setInstancedVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices, const std::vector<InstancedTransform> &instanceTransforms) override;
	void updateInstancedVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices, const std::vector<InstancedTransform> &instanceTransforms) override;

private:
	GLuint vBufferSize;
	GLuint iBufferSize;
	GLuint usage;

	GLuint instancedModelMatriciesBuffer;

	void updateBufferParams();
};

#endif