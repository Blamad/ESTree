#ifndef RENDERER_H
#define RENDERER_H

#include "VertexArray.h"
#include "Shader.h"

class Renderer {
public:
	virtual ~Renderer() = default;
	virtual void renderObject(VertexArray &vertexArray, Shader *shader) = 0;
	virtual void renderInstancedObject(VertexArray &vertexArray, Shader *shader, int &amount) = 0;
};

#endif // !RENDERER_H

