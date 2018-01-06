#ifndef LINDENMAYER_TREE_MESH_GENERATOR_H
#define LINDENMAYER_TREE_MESH_GENERATOR_H

#include <boost/thread/thread.hpp>

#include "Mesh.h"

class VertexGenerationAttributes;

class LindenmayerTreeMeshGenerator {
public:
	LindenmayerTreeMeshGenerator(Mesh *mesh, int &ringDensity);

	void enqueueGenerationData(float &radius, mat4 &transform, int textureY);
	void generateMesh();

private:
	vector<VertexGenerationAttributes> threadGenerationData;
	float textureXStep;
	int ringDensity; 
	Mesh *mesh;

	void computeRingPoint(int startIndex, int endIndex, Mesh *mesh);
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