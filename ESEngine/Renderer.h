#ifndef RENDERER_H
#define RENDERER_H

#include "VertexArray.h"
#include "Shader.h"

class Renderer {
public:
	virtual void renderObject(const VertexArray &vertexArray, Shader &shader) = 0;
};

#endif // !RENDERER_H

