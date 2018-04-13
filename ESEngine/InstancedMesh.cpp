#include "InstancedMesh.h"
#include <boost/foreach.hpp>

void InstancedMesh::draw(Renderer &renderer) {
	BOOST_FOREACH(shared_ptr<Shader> shader, shaders) {
		if (!shader->active)
			continue;

		GLuint program = shader->program;
		shader->use();

		if (!initialized) {
			glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, glm::value_ptr(material.ambient));
			glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, glm::value_ptr(material.diffuse));
			glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, glm::value_ptr(material.specular));
			glUniform1f(glGetUniformLocation(program, "material.shininess"), material.shininess);
			glUniform1f(glGetUniformLocation(program, "material.texDiffuse"), 0);
			glUniform1f(glGetUniformLocation(program, "material.texSpecular"), 1);
			glUniform1i(glGetUniformLocation(program, "directionalShadingSamples[0]"), 2);
			initialized;
		}

		if (material.texDiffuse != nullptr) {
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(glGetUniformLocation(program, "material.texDiffuse"), 0);
			glBindTexture(GL_TEXTURE_2D, material.texDiffuse->textureBuffer->id);
		}

		if (material.texSpecular != nullptr) {
			glActiveTexture(GL_TEXTURE1);
			glUniform1i(glGetUniformLocation(program, "material.texSpecular"), 1);
			glBindTexture(GL_TEXTURE_2D, material.texSpecular->textureBuffer->id);
		}
		int numberOfInstances = instanceMatricies.size();
		renderer.renderInstancedObject(*vertexArray, shader.get(), numberOfInstances);

		if (material.texDiffuse != nullptr) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		if (material.texSpecular != nullptr) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

InstancedMesh::InstancedMesh(vector<Vertex> &vertices, vector<int> &indices, vector<InstancedTransform> &instancedTransforms, shared_ptr<Shader> shader, int vBufferSize, int iBufferSize, int bufferUsage) : Mesh(shader, bufferUsage) {
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

void InstancedMesh::setupMesh()
{
	BOOST_FOREACH(shared_ptr<Shader> shader, shaders) {
		vertexArray->setInstancedVertexArray(vertices, indices, instanceMatricies);
		shader->registerMatriciesUBO();
		shader->registerLightsUBO();
	}
}
