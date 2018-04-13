#ifndef INSTANCED_MESH_H
#define INSTANCED_MESH_H

#include "Mesh.h"
#include "InstancedTransform.h"

using namespace std;

class InstancedMesh : public Mesh {
public:

	InstancedMesh(vector<Vertex> &vertices, vector<int> &indices, vector<InstancedTransform> &instancedTransforms, shared_ptr<Shader> shader, int vBufferSize = -1, int iBufferSize = -1, int bufferUsage = GL_STATIC_DRAW);
	~InstancedMesh();

	void draw(Renderer &renderer) override;
	void addInstance(InstancedTransform instancedTransform);
	void updateMesh();

protected:
	vector<InstancedTransform> instanceMatricies;

	void setupMesh();
};

#endif