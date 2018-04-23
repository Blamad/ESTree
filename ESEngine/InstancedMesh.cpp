#include "InstancedMesh.h"

Logger InstancedMesh::logger("InstancedMesh");

void InstancedMesh::draw(Renderer &renderer) {
	BOOST_FOREACH(shared_ptr<Shader> shader, shaders) {
		draw(renderer, shader.get());
	}
}

void InstancedMesh::draw(Renderer &renderer, Shader *shader) {
	if (!shader->active) {
		logger.log(WARN, "Trying to draw with inactive shader!");
		return;
	}

	GLuint program = shader->program;
	shader->use();

	if (!shader->isInitializedBy(identifier())) {
		shader->registerUniform("material.ambient");
		shader->registerUniform("material.diffuse");
		shader->registerUniform("material.specular");
		shader->registerUniform("material.shininess");
		shader->registerUniform("material.texDiffuse");
		shader->registerUniform("material.texSpecular");
		shader->registerUniform("directionalShadingSamples[0]");

		shader->registerSubroutine("shadowDepthPass", GL_FRAGMENT_SHADER);
		shader->registerSubroutine("renderPass", GL_FRAGMENT_SHADER);
		shader->registerSubroutine("instancedMesh", GL_VERTEX_SHADER);
		shader->registerSubroutine("singleMesh", GL_VERTEX_SHADER);

		glUniform3fv(shader->getUniformLocation("material.ambient"), 1, glm::value_ptr(material.ambient));
		glUniform3fv(shader->getUniformLocation("material.diffuse"), 1, glm::value_ptr(material.diffuse));
		glUniform3fv(shader->getUniformLocation("material.specular"), 1, glm::value_ptr(material.specular));
		glUniform1f(shader->getUniformLocation("material.shininess"), material.shininess);
		glUniform1i(shader->getUniformLocation("material.texDiffuse"), 0);
		glUniform1i(shader->getUniformLocation("material.texSpecular"), 1);
		glUniform1i(shader->getUniformLocation("directionalShadingSamples[0]"), 2);
		shader->setInitializedBy(identifier());
	}

	shader->setShaderSubroutine(INSTANCED_MESH_MODE);
	shader->updateShaderSubroutine();

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

	int numberOfInstances = instanceMatricies.size();
	renderer.renderInstancedObject(*vertexArray, shader, numberOfInstances);

	if (material.texDiffuse != nullptr) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (material.texSpecular != nullptr) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

InstancedMesh::InstancedMesh(vector<Vertex> &vertices, vector<int> &indices, vector<InstancedTransform> &instancedTransforms, shared_ptr<Shader> shader, int vBufferSize, int iBufferSize, int bufferUsage) : Renderable(shader) {
	if (vBufferSize == -1)
		vBufferSize = vertices.size();
	if (iBufferSize == -1)
		iBufferSize = indices.size();
	this->instanceMatricies = vector<InstancedTransform>(instancedTransforms);
	this->vertices = vector<Vertex>(vertices);
	this->indices = vector<int>(indices);

	vertexArray = unique_ptr<VertexArray>(new GLVertexArray(vBufferSize, iBufferSize, bufferUsage));
	this->bufferUsage = bufferUsage;
	setupMesh();
}

InstancedMesh::~InstancedMesh() {

}

void InstancedMesh::addInstance(InstancedTransform instancedTransform) {
	instanceMatricies.push_back(instancedTransform);
}

void InstancedMesh::updateMesh() {
	vertexArray->updateInstancedVertexArray(vertices, indices, instanceMatricies);
}

void InstancedMesh::setupMesh() {
	BOOST_FOREACH(shared_ptr<Shader> shader, shaders) {
		vertexArray->setInstancedVertexArray(vertices, indices, instanceMatricies);
		shader->registerMatriciesUBO();
		shader->registerLightsUBO();
	}
}
