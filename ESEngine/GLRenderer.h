#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include "GLVertexArray.h"
#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>

class GLRenderer : public Renderer {
	
public:
	void renderObject(VertexArray &vertexArray, Shader *shader);
	void renderInstancedObject(VertexArray &vertexArray, Shader *shader, int &amount);
};

#endif // !GLRENDERER_H
