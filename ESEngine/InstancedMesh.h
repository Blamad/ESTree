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

using namespace std;

class InstancedMesh : public Renderable {
public:
	Material material;
	vector<Vertex> vertices;
	vector<int> indices;
	vector<InstancedTransform> instanceMatricies;
	mat4 initialTransfom;

	InstancedMesh(vector<Vertex> &vertices, vector<int> &indices, vector<InstancedTransform> &instancedTransforms, shared_ptr<Shader> shader, int vBufferSize = -1, int iBufferSize = -1, int bufferUsage = GL_STATIC_DRAW);
	~InstancedMesh();

	void draw(Renderer &renderer) override;
	void draw(Renderer &renderer, Shader* shader) override;

	void addInstance(InstancedTransform instancedTransform);
	void updateMesh();

protected:
	static Logger logger;

	bool initialized = false;
	int bufferUsage = GL_STATIC_DRAW;
	unique_ptr<VertexArray> vertexArray;

	void setupMesh();

	int identifier() override { return 2; }
};

#endif