#include "Skybox.h"
#include "GLVertexArray.h"

Skybox::Skybox(string* paths, Shader shader) : Renderable(shader)
{
	this->loadSkybox(paths);
}

void Skybox::loadSkybox(string* paths)
{
	cubeMap = TextureManager::getInstance().createCubeMap(paths);
	generateVertexArray();
}

void Skybox::draw(Renderer &renderer, Shader &shader) {
	glDepthFunc(GL_LEQUAL);

	shader.use();

	if (!initialized) {
		glUniform1i(glGetUniformLocation(shader.program, "cubemap"), 0);
		initialized = true;
	}

	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(getModelMatrix()));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->textureBuffer->id);
	renderer.renderObject(*vertexArray, shaders[0]);
	glDepthFunc(GL_LESS);
}

void Skybox::draw(Renderer& renderer) {
	draw(renderer, shaders[0]);
}

void Skybox::generateVertexArray()
{
	vector<Vertex> vertices = {
		//front - z
		Vertex::createVertex(vec3(1.0f, -1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 0)),
		Vertex::createVertex(vec3(1.0f,  1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 1)),
		Vertex::createVertex(vec3(-1.0f,  1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 1)),
		Vertex::createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 0)),
		//back - z
		Vertex::createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),	vec2(0, 0)),
		Vertex::createVertex(vec3(-1.0f,  1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(0, 1)),
		Vertex::createVertex(vec3(1.0f,  1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 1)),
		Vertex::createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 0)),
		//left - x
		Vertex::createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(-1.0f, 0.0f, 0.0f),	vec2(0, 1)),
		Vertex::createVertex(vec3(-1.0f,  1.0f,  1.0f),	vec3(-1.0f, 0.0f, 0.0f),	vec2(1, 1)),
		Vertex::createVertex(vec3(-1.0f,  1.0f, -1.0f),	vec3(-1.0f, 0.0f, 0.0f),	vec2(1, 0)),
		Vertex::createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(-1.0f, 0.0f, 0.0f),	vec2(0, 0)),
		//right - x
		Vertex::createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(0, 0)),
		Vertex::createVertex(vec3(1.0f,  1.0f, -1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(1, 0)),
		Vertex::createVertex(vec3(1.0f,  1.0f,  1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(1, 1)),
		Vertex::createVertex(vec3(1.0f, -1.0f,  1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(0, 1)),
		//top - y
		Vertex::createVertex(vec3(1.0f, 1.0f,  1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(1, 1)),
		Vertex::createVertex(vec3(1.0f, 1.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(1, 0)),
		Vertex::createVertex(vec3(-1.0f, 1.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(0, 0)),
		Vertex::createVertex(vec3(-1.0f, 1.0f,  1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(0, 1)),
		//bottom - y
		Vertex::createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(0.0f, -1.0f, 0.0f),	vec2(0, 1)),
		Vertex::createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f),	vec2(0, 0)),
		Vertex::createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(0.0f, -1.0f, 0.0f),	vec2(1, 0)),
		Vertex::createVertex(vec3(1.0f, -1.0f,  1.0f),	vec3(0.0f, -1.0f, 0.0f),	vec2(1, 1)),
	};

	vector<int> indices = {
		3, 1, 0, 3, 2, 1,
		7, 5, 4, 7, 6, 5,
		11, 9, 8, 11, 10, 9,
		15, 13, 12, 15, 14, 13,
		19, 17, 16, 19, 18, 17,
		23, 21, 20, 23, 22, 21
	};

	vertexArray = unique_ptr<VertexArray>(new GLVertexArray(vertices.size(), indices.size(), GL_STATIC_DRAW));
	vertexArray->setVertexArray(vertices, indices);
	shaders[0].registerMatriciesUBO();
	shaders[0].use();
	glUniform1i(glGetUniformLocation(shaders[0].program, "cubemap"), 0);
}