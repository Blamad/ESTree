#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "Component.h"
#include "Renderer.h"
#include "Shader.h"

#include <boost/foreach.hpp>

class Renderable : public Component {
public:
	std::vector<std::shared_ptr<Shader>> shaders;
	virtual void draw(Renderer &renderer) = 0;
	virtual void draw(Renderer &renderer, Shader *shader) = 0;

protected:
	virtual int identifier() = 0;

	Renderable(std::shared_ptr<Shader> shader) : Component(RENDERABLE) {
		this->shaders.push_back(shader);
	};

	Renderable(std::vector<std::shared_ptr<Shader>> shaders) : Component(RENDERABLE) {
		BOOST_FOREACH(std::shared_ptr<Shader> shader, shaders) {
			this->shaders.push_back(shader);
		}
	};

	void addShader(std::shared_ptr<Shader> shader) {
		this->shaders.push_back(shader);
	}
};

#endif