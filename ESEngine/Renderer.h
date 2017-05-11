#ifndef RENDERER_H
#define RENDERER_H

#include "VertexArray.h"
#include "Shader.h"

class Renderer {
public:
	virtual void renderObject(VertexArray &vertexArray, Shader &shader) = 0;
};

#endif // !RENDERER_H

