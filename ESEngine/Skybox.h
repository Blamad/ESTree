#ifndef SKYBOX_H
#define SKYBOX_H

#include "ShaderManager.h"
#include "Renderable.h"
#include "TextureManager.h"
#include "GLVertexArray.h"

class Skybox : public Renderable
{
public:
	Skybox(std::string* paths);
	Skybox(std::string* paths, std::shared_ptr<Shader> shader);

	void draw(Renderer &renderer);
	void draw(Renderer &renderer, Shader *shader);
	
	void updatePosition(glm::vec3 position) {
		this->position = position;
	}

protected:
	int identifier() { return 0; }

private:
	std::shared_ptr<Texture> cubeMap;
	std::shared_ptr<VertexArray> vertexArray;

	glm::vec3 position;
	bool initialized = false;

	void loadSkybox(std::string* paths);
	void setupSkybox();
	void generateVertexArray();

	glm::mat4 getModelMatrix() {
		return translate(glm::mat4(), position);
	}
};

#endif