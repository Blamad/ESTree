#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "GLVertexArray.h"
#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>


class GLRenderer : public Renderer {
	
public:

	void render();
	void setViewProjectionMatricies(glm::mat4 projection, glm::mat4 view);
};

#endif // !GLRENDERER_H
