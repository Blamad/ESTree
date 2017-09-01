#ifndef LINDENMAYER_TREE_H
#define LINDENMAYER_TREE_H

#include <string>
#include <stack>
#include <vector>

#include <boost/random.hpp>
#include <time.h>
#include <boost/generator_iterator.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "Segment.h"
#include "InstancedMesh.h"
#include "LindenmayerTreeParams.h"

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

class LindenmayerTree : public GameObject {
public:
	LindenmayerTree(LindenmayerTreeParams &params, Material &material = Material::bark5(), Material &leavesMaterial = Material::leaves2(), bool useMeshWiring = false);
	void generate();
	bool meshWiring;

protected:
	Material material;
	shared_ptr<Mesh> mesh;
	shared_ptr<Segment> root;

	int vBufferSize = 1000;
	int iBufferSize = 4000;
	
	int segments;
	float textureXStep;

private:
	LindenmayerTreeParams params;
	string product;
	vector<shared_ptr<Segment>> segmentsVec;
	Material leavesMaterial;

	static boost::variate_generator<boost::mt19937, boost::uniform_real<>> randomGenerator;

	void generateTree();
	
	//Parsing l-data
	void generateMeshSkeleton();
	string parseRule(string &symbol, int &depth);
	float getNumericParameter(string product, int index);
	float returnNewIndexAfterParameter(string product, int index);
	
	//Mesh and vertices stuff
	void createMeshComponent();
	void generateMeshData();
	void createRoot(SegmentTransform &transform);
	shared_ptr<Segment> createSegment(shared_ptr<Segment> parent, SegmentTransform &transform);
	shared_ptr<Segment> createSegment(shared_ptr<Segment> parent, float &radius, float &length, quat &rotation);

	Vertex createVertex(vec3 position, vec3 normal, vec2 texCoords);

	//Parallel generation
	//2.0
	vector<VertexGenerationAttributes> threadGenerationData;
	void enqueueGenerationData(float &radius, mat4 &transform, int textureY);
	void generateVertices();
	void computeRingPoint(int startIndex, int endIndex);
	
	//Leaves generation
	void generateInstancedLeaves();
	shared_ptr<InstancedMesh> createInstancedLeavesCubeMesh(vector<InstancedTransform> &instancedTransforms);
	shared_ptr<InstancedMesh> createInstancedLeavesPanelMesh(vector<InstancedTransform> &instancedTransforms);
	
	//Stuff
	float toRadians(float angle) {
		return (angle * 3.14159265359) / 180;
	}
};

#endif