#ifndef INSTANCED_MESH_H
#define INSTANCED_MESH_H

#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <boost/foreach.hpp>

#include "ShaderManager.h"
#include "Renderable.h"
#include "Renderer.h"
#include "InstancedTransform.h"
#include "Material.h"

#include "TextureBuffer.h"
#include "GLVertexArray.h"
#include "Context.h"

#include "Logger.h"

class InstancedMesh : public Renderable {
public:
	Material material;
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	std::vector<InstancedTransform> instanceMatricies;
	glm::mat4 initialTransfom;

	InstancedMesh(std::vector<Vertex> &vertices, std::vector<int> &indices, std::vector<InstancedTransform> &instancedTransforms, std::shared_ptr<Shader> shader, int vBufferSize = 0, int iBufferSize = 0, int bufferUsage = GL_STATIC_DRAW);
	~InstancedMesh();

	void draw(Renderer &renderer) override;
	void draw(Renderer &renderer, Shader* shader) override;

	void addInstance(InstancedTransform instancedTransform);
	void updateMesh();

protected:
	static Logger logger;

	bool initialized = false;
	int bufferUsage = GL_STATIC_DRAW;
	std::unique_ptr<VertexArray> vertexArray;

	void setupMesh();

	int identifier() override { return 2; }
};

#endif