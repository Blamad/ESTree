#ifndef L_TREE_H
#define L_TREE_H

#include <string>
#include <stack>

#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/foreach.hpp>
#include <time.h>

#include "Tree.h"
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

class LTree : public GameObject {
public:
	LTree(LindenmayerTreeParams &params, Material &material = Material::bark5(), Material &leavesMaterial = Material::leaves2(), bool useMeshWiring = false);
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
	void createRoot(float &radius, quat &rotation);
	shared_ptr<Segment> createSegment(shared_ptr<Segment> parent, float &radius, float &length, quat &rotation);

	Vertex createVertex(vec3 position, vec3 normal, vec2 texCoords);

	//Parallel generation
	//2.0
	vector<VertexGenerationAttributes> threadGenerationData;
	void enqueueGenerationData(float &radius, mat4 &transform, int textureY);
	void generateVertices();
	void computeRingPoint(int startIndex, int endIndex);

	//1.0
	void createParallelRing(float &radius, mat4 &transform, int textureY);
	void computeRingPoint(int verticesOffset, int vertexNumber, float &theta, float &radius, mat4 &transform, int &textureY);
	
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