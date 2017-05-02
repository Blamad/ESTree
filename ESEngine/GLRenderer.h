#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "GLVertexArray.h"
#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>

class GLRenderer : public Renderer {
	
public:
	void renderObject(const VertexArray &vertexArray, Shader &shader);
};

#endif // !GLRENDERER_H
