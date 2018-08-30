#ifndef LINDENMAYER_TREE_MESH_GENERATOR_H
#define LINDENMAYER_TREE_MESH_GENERATOR_H

#include <boost/thread/thread.hpp>

#include "Mesh.h"
#include "InstancedMesh.h"

class VertexGenerationAttributes;

class LindenmayerTreeMeshGenerator {
public:
	LindenmayerTreeMeshGenerator(Mesh *mesh, int &ringDensity);

	void enqueueGenerationData(float &radius, mat4 &transform, int textureY);
	void createIndiciesForSegment();
	int getVerticesLength() { return mesh->vertices.size(); }
	int getIndicesLength() { return mesh->indices.size(); }
	int getVerticesPerRing() { return ringDensity; }

	void generateMesh();

	//Leaves generation
	void generateLeaves();
	void generateInstancedLeaves();
	
private:
	vector<VertexGenerationAttributes> threadGenerationData;
	float textureXStep;
	int ringDensity; 
	Mesh *mesh;

	void computeRingPoint(int startIndex, int endIndex, Mesh *mesh);

	shared_ptr<Mesh> generateLeaf();
	shared_ptr<InstancedMesh> createInstancedLeavesPanelMesh(vector<InstancedTransform> &instancedTransforms);
};

class VertexGenerationAttributes {
public:
	VertexGenerationAttributes(int verticesOffset, int vertexNumber, float &theta, float &radius, mat4 &transform, int textureY)
		: verticesOffset(verticesOffset), vertexNumber(vertexNumber), theta(theta), radius(radius), transform(transform), textureY(textureY) { };

	int verticesOffset;
	int vertexNumber;
	float theta;
	float radius;
	mat4 transform;
	int textureY;
};

#endif