#ifndef LINDENMAYER_TREE_MESH_GENERATOR_H
#define LINDENMAYER_TREE_MESH_GENERATOR_H

#include <boost/thread/thread.hpp>

#include "Engine/GameObject/Mesh.h"
#include "Engine/GameObject/InstancedMesh.h"

class VertexGenerationAttributes;

class LindenmayerTreeMeshGenerator {
public:
	LindenmayerTreeMeshGenerator(Mesh *mesh, int &ringDensity);

	void enqueueGenerationData(float &radius, glm::mat4 &transform, int textureY);
	void createIndiciesForSegment();
	size_t getVerticesLength() { return mesh->vertices.size(); }
	size_t getIndicesLength() { return mesh->indices.size(); }
	int getVerticesPerRing() { return ringDensity; }

	void generateMesh();

	//Leaves generation
	void generateLeaves();
	void generateInstancedLeaves();
	
private:
	std::vector<VertexGenerationAttributes> threadGenerationData;
	float textureXStep;
	int ringDensity; 
	Mesh *mesh = nullptr;

	void computeRingPoint(int startIndex, int endIndex, Mesh *mesh);

	std::shared_ptr<Mesh> generateLeaf();
	std::shared_ptr<InstancedMesh> createInstancedLeavesPanelMesh(std::vector<InstancedTransform> &instancedTransforms);
};

class VertexGenerationAttributes {
public:
	VertexGenerationAttributes(int verticesOffset, int vertexNumber, float &theta, float &radius, glm::mat4 &transform, int textureY)
		: verticesOffset(verticesOffset), vertexNumber(vertexNumber), theta(theta), radius(radius), transform(transform), textureY(textureY) { };

	int verticesOffset;
	int vertexNumber;
	float theta;
	float radius;
	glm::mat4 transform = glm::mat4(1);
	int textureY;
};

#endif