#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "Vertex.h"
#include "InstancedTransform.h"
#include <vector>

class VertexArray {
public:
	virtual void setVertexArray(const std::vector<Vertex> & vertices, const std::vector<int> & indices) = 0;
	virtual void updateVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices) = 0;
	virtual void setInstancedVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices, const std::vector<InstancedTransform> &instanceTransforms) = 0;
	virtual void updateInstancedVertexArray(const std::vector<Vertex> &vertices, const std::vector<int> &indices, const std::vector<InstancedTransform> &instanceTransforms) = 0;
};

#endif