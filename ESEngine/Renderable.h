#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "Component.h"
#include "Renderer.h"
#include "Shader.h"

class Renderable : public Component {
public:
	Shader shader;
	virtual void draw(Renderer &renderer) = 0;
protected:
	Renderable(Shader &shader) : shader(shader), Component(RENDERABLE) { };
};

#endif