#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "Vertex.h"
#include <vector>

class VertexArray {
public:
	virtual void setVertexArray(const std::vector<Vertex> & vertices, const std::vector<int> & indices) = 0;
};

#endif