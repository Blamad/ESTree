#ifndef SKYBOX_H
#define SKYBOX_H

#include "ShaderManager.h"
#include "Renderable.h"
#include "TextureManager.h"
#include "GLVertexArray.h"

class Skybox : public Renderable
{
public:
	Skybox(string* paths);
	Skybox(string* paths, shared_ptr<Shader> shader);

	void draw(Renderer &renderer);
	void draw(Renderer &renderer, Shader *shader);
	
	void updatePosition(vec3 position) {
		this->position = position;
	}

protected:
	int identifier() { return 0; }

private:
	shared_ptr<Texture> cubeMap;
	shared_ptr<VertexArray> vertexArray;

	vec3 position;
	bool initialized = false;

	void loadSkybox(string* paths);
	void setupSkybox();
	void generateVertexArray();

	glm::mat4 getModelMatrix() {
		return glm::translate(glm::mat4(), position);
	}
};

#endif