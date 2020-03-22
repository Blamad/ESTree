#include "Skybox.h"

Skybox::Skybox(std::string* paths) : Renderable(ShaderManager::getShader("Shaders/SkyboxShader.vert", "Shaders/SkyboxShader.frag")) {
	this->loadSkybox(paths);
}

Skybox::Skybox(std::string* paths, std::shared_ptr<Shader> shader) : Renderable(shader) {
	this->loadSkybox(paths);
}

void Skybox::loadSkybox(std::string* paths) {
	cubeMap = TextureManager::getInstance().createCubeMap(paths);
	generateVertexArray();
	setupSkybox();
}

void Skybox::draw(Renderer &renderer, Shader *shader) {
	glDepthFunc(GL_LEQUAL);

	shader->use();

	if (!shader->isInitializedBy(identifier())) {
		shader->registerUniform("cubemap");
		shader->registerUniform("model");

		glUniform1i(shader->getUniformLocation("cubemap"), 0);
		shader->setInitializedBy(identifier());
	}

	glUniformMatrix4fv(shader->getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(getModelMatrix()));

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(shader->getUniformLocation("cubemap"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->textureBuffer->id);
	renderer.renderObject(*vertexArray, shaders[0].get());
	glDepthFunc(GL_LESS);
}

void Skybox::draw(Renderer& renderer) {
	BOOST_FOREACH(std::shared_ptr<Shader> shader, shaders) {
		draw(renderer, shader.get());
	}
}

void Skybox::setupSkybox() {
	BOOST_FOREACH(std::shared_ptr<Shader> shader, shaders) {
		shader->registerMatriciesUBO();
	}
}

void Skybox::generateVertexArray() {
	std::vector<Vertex> vertices = {
		//front - z
		Vertex::createVertex(glm::vec3(1.0f, -1.0f,  1.0f),		glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec2(1, 0)),
		Vertex::createVertex(glm::vec3(1.0f,  1.0f,  1.0f),		glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec2(1, 1)),
		Vertex::createVertex(glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec2(0, 1)),
		Vertex::createVertex(glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec2(0, 0)),
		//back - z
		Vertex::createVertex(glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(0, 0)),
		Vertex::createVertex(glm::vec3(-1.0f,  1.0f, -1.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(0, 1)),
		Vertex::createVertex(glm::vec3(1.0f,  1.0f, -1.0f),		glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(1, 1)),
		Vertex::createVertex(glm::vec3(1.0f, -1.0f, -1.0f),		glm::vec3(0.0f, 0.0f, -1.0f),	glm::vec2(1, 0)),
		//left - x
		Vertex::createVertex(glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),	glm::vec2(0, 1)),
		Vertex::createVertex(glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),	glm::vec2(1, 1)),
		Vertex::createVertex(glm::vec3(-1.0f,  1.0f, -1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),	glm::vec2(1, 0)),
		Vertex::createVertex(glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),	glm::vec2(0, 0)),
		//right - x
		Vertex::createVertex(glm::vec3(1.0f, -1.0f, -1.0f),		glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec2(0, 0)),
		Vertex::createVertex(glm::vec3(1.0f,  1.0f, -1.0f),		glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec2(1, 0)),
		Vertex::createVertex(glm::vec3(1.0f,  1.0f,  1.0f),		glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec2(1, 1)),
		Vertex::createVertex(glm::vec3(1.0f, -1.0f,  1.0f),		glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec2(0, 1)),
		//top - y
		Vertex::createVertex(glm::vec3(1.0f, 1.0f,  1.0f),		glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(1, 1)),
		Vertex::createVertex(glm::vec3(1.0f, 1.0f, -1.0f),		glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(1, 0)),
		Vertex::createVertex(glm::vec3(-1.0f, 1.0f, -1.0f),		glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(0, 0)),
		Vertex::createVertex(glm::vec3(-1.0f, 1.0f,  1.0f),		glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec2(0, 1)),
		//bottom - y
		Vertex::createVertex(glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec3(0.0f, -1.0f, 0.0f),	glm::vec2(0, 1)),
		Vertex::createVertex(glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec3(0.0f, -1.0f, 0.0f),	glm::vec2(0, 0)),
		Vertex::createVertex(glm::vec3(1.0f, -1.0f, -1.0f),		glm::vec3(0.0f, -1.0f, 0.0f),	glm::vec2(1, 0)),
		Vertex::createVertex(glm::vec3(1.0f, -1.0f,  1.0f),		glm::vec3(0.0f, -1.0f, 0.0f),	glm::vec2(1, 1)),
	};

	std::vector<int> indices = {
		3, 1, 0, 3, 2, 1,
		7, 5, 4, 7, 6, 5,
		11, 9, 8, 11, 10, 9,
		15, 13, 12, 15, 14, 13,
		19, 17, 16, 19, 18, 17,
		23, 21, 20, 23, 22, 21
	};

	vertexArray = std::unique_ptr<VertexArray>(new GLVertexArray(GL_STATIC_DRAW));
	vertexArray->setVertexArray(vertices, indices);
}