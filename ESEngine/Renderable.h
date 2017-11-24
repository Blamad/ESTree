#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "Component.h"
#include "Renderer.h"
#include "Shader.h"

#include <boost/foreach.hpp>

class Renderable : public Component {
public:
	vector<Shader> shaders;
	virtual void draw(Renderer &renderer) = 0;
	virtual void draw(Renderer &renderer, Shader &shader) = 0;

protected:
	Renderable(Shader &shader) : Component(RENDERABLE) {
		this->shaders.push_back(shader);
	};

	Renderable(vector<Shader> &shaders) : Component(RENDERABLE) {
		BOOST_FOREACH(Shader shader, shaders) {
			this->shaders.push_back(shader);
		}
	};
};

#endif