#ifndef SKYBOX_H
#define SKYBOX_H

#include "Renderable.h"
#include "TextureManager.h"
#include "GLVertexArray.h"

class Skybox : public Renderable
{
public:
	Skybox(string* paths, Shader shader);
	void draw(Renderer &renderer);
	
	void updatePosition(vec3 position) {
		this->position = position;
	}

private:
	shared_ptr<Texture> cubeMap;
	shared_ptr<VertexArray> vertexArray;

	vec3 position;
	bool initialized = false;

	void loadSkybox(string* paths);
	void generateVertexArray();

	glm::mat4 getModelMatrix() {
		return glm::translate(glm::mat4(), position);
	}
};

#endif