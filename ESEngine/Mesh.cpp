#include "Mesh.h"

Logger Mesh::logger("Mesh");

void Mesh::draw(Renderer &renderer) {
	BOOST_FOREACH(std::shared_ptr<Shader> shader, shaders) {
		draw(renderer, shader.get());
	}
	if (isNormalsShaderEnabled) {
		draw(renderer, getNormalsShader());
	}
	if (isMeshWiringShaderEnabled) {
		draw(renderer, getMeshWiringShader());
	}
}

void Mesh::draw(Renderer &renderer, Shader *shader) {
	if (!shader->active) {
		logger.log(LOG_WARN, "Trying to draw with inactive shader!");
		return;
	}

	BOOST_FOREACH(std::shared_ptr<GameObject> child, parent->children) {
		if (child->getComponent(RENDERABLE) != nullptr) {
			((Renderable*)child->getComponent(RENDERABLE))->draw(renderer, shader);
		}
	}

	Transform *transform = (Transform*)this->getComponent(TRANSFORM);

	shader->use();

	if (!shader->isInitializedBy(identifier())) {
		shader->registerUniform("material.ambient");
		shader->registerUniform("material.diffuse");
		shader->registerUniform("material.specular");
		shader->registerUniform("material.shininess");
		shader->registerUniform("material.texDiffuse");
		shader->registerUniform("material.texSpecular");
		shader->registerUniform("directionalShadingSamples[0]");
		shader->registerUniform("model");
		shader->registerUniform("normalModel");
		shader->registerUniform("time");

		shader->registerSubroutine("shadowDepthPass", GL_FRAGMENT_SHADER);
		shader->registerSubroutine("renderPass", GL_FRAGMENT_SHADER);
		shader->registerSubroutine("instancedMesh", GL_VERTEX_SHADER);
		shader->registerSubroutine("singleMesh", GL_VERTEX_SHADER);

		glUniform1i(shader->getUniformLocation("material.texDiffuse"), 0);
		glUniform1i(shader->getUniformLocation("material.texSpecular"), 1);
		glUniform1i(shader->getUniformLocation("directionalShadingSamples[0]"), 2);

		shader->setInitializedBy(identifier());
	}

	glUniform3fv(shader->getUniformLocation("material.ambient"), 1, glm::value_ptr(material.ambient));
	glUniform3fv(shader->getUniformLocation("material.diffuse"), 1, glm::value_ptr(material.diffuse));
	glUniform3fv(shader->getUniformLocation("material.specular"), 1, glm::value_ptr(material.specular));
	glUniform1f(shader->getUniformLocation("material.shininess"), material.shininess);
	glUniform1f(shader->getUniformLocation("time"), Context::getTime());

	shader->setShaderSubroutine("singleMesh");

	if (material.texDiffuse != nullptr) {
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(shader->getUniformLocation("material.texDiffuse"), 0);
		glBindTexture(GL_TEXTURE_2D, material.texDiffuse->textureBuffer->id);
	}

	if (material.texSpecular != nullptr) {
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(shader->getUniformLocation("material.texSpecular"), 1);
		glBindTexture(GL_TEXTURE_2D, material.texSpecular->textureBuffer->id);
	}

	glUniformMatrix4fv(shader->getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(transform->getModelMatrix()));
	glUniformMatrix3fv(shader->getUniformLocation("normalModel"), 1, GL_FALSE, glm::value_ptr(transform->getNormalModelMatrix()));

	renderer.renderObject(*vertexArray, shader);

	if (material.texDiffuse != nullptr) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (material.texSpecular != nullptr) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<int> &indices, std::shared_ptr<Shader> shader, int vBufferSize, int iBufferSize, int bufferUsage) : vertices(vertices), indices(indices), Renderable(shader) {
	if (vBufferSize == -1)
		vBufferSize = vertices.size();
	if (iBufferSize == -1)
		iBufferSize = indices.size();

	vertexArray = std::unique_ptr<VertexArray>(new GLVertexArray(vBufferSize, iBufferSize, bufferUsage));
	this->bufferUsage = bufferUsage;
	setupMeshes();
}

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<int> &indices, std::vector<std::shared_ptr<Shader>> shaders, int vBufferSize, int iBufferSize, int bufferUsage) : vertices(vertices), indices(indices), Renderable(shaders) {
	if (vBufferSize == -1)
		vBufferSize = vertices.size();
	if (iBufferSize == -1)
		iBufferSize = indices.size();

	vertexArray = std::unique_ptr<VertexArray>(new GLVertexArray(vBufferSize, iBufferSize, bufferUsage));
	this->bufferUsage = bufferUsage;
	setupMeshes();
}

Mesh::~Mesh() {

}

void Mesh::setupMeshes() {
	BOOST_FOREACH(std::shared_ptr<Shader> shader, shaders) {
		setupMesh(shader);
	}
}

void Mesh::updateMesh() {
	vertexArray->updateVertexArray(vertices, indices);
}

void Mesh::setupMesh(std::shared_ptr<Shader> shader) {
	vertexArray->setVertexArray(vertices, indices);
	shader->registerMatriciesUBO();
	shader->registerLightsUBO();
}

void Mesh::addNewShader(std::shared_ptr<Shader> shader) {
	addShader(shader);
	setupMesh(shader);
}

void Mesh::showMeshWiring() {
	isMeshWiringShaderEnabled = true;
}

void Mesh::showNormalVisualisation() {
	isNormalsShaderEnabled = true;
}

void Mesh::hideMeshWiring() {
	isMeshWiringShaderEnabled = false;
}

void Mesh::hideNormalVisualisation() {
	isNormalsShaderEnabled = false;
}