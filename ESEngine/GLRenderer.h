#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "GLVertexArray.h"
#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>

class GLRenderer : public Renderer {
	
public:
	void renderObject(VertexArray &vertexArray, Shader &shader);
	void renderInstancedObject(VertexArray &vertexArray, Shader &shader, int &amount);
};

#endif // !GLRENDERER_H
