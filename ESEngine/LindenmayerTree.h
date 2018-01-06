#ifndef LINDENMAYER_TREE_H
#define LINDENMAYER_TREE_H

#include <string>
#include <stack>
#include <vector>

#include <time.h>
#include <boost/generator_iterator.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Segment.h"
#include "InstancedMesh.h"
#include "LindenmayerTreeParams.h"
#include "LindenmayerTreeSolver.h"
#include "LindenmayerTreeMeshGenerator.h"

#include "Logger.h"

class LindenmayerTree : public GameObject {
public:
	LindenmayerTree(LindenmayerTreeParams &params, Material &material = Material::bark5(), Material &leavesMaterial = Material::leaves2(), bool useMeshWiring = false, bool normalVisualisation = false);
	void generate();
	bool meshWiring;
	bool normalVisualisation;

protected:
	Material material;
	shared_ptr<Mesh> mesh;
	shared_ptr<Segment> root;

	int vBufferSize = 1000;
	int iBufferSize = 4000;
	const float branchLeafOverlappingFactor = .8f;
	
	int ringDensity;

private:
	static Logger logger;

	vector<shared_ptr<Segment>> segmentsVec;
	unique_ptr<LindenmayerTreeMeshGenerator> meshGenerator;
	LindenmayerTreeParams params;
	Material leavesMaterial;
	string product;

	static boost::variate_generator<boost::mt19937, boost::uniform_real<>> randomGenerator;

	void generateTree();

	//Parsing l-data
	void generateMeshSkeleton();
	float getNumericParameter(string product, int index);
	float returnNewIndexAfterParameter(string product, int index);
	
	//Mesh and vertices stuff
	void generateTreeMesh();
	void createMeshComponent();
	void generateMeshData();
	quat test(SegmentTransform transform);
	float calculateRollAngle(shared_ptr<Segment> parent, SegmentTransform & transform);
	void createRoot(SegmentTransform &transform);
	shared_ptr<Segment> createSegment(shared_ptr<Segment> parent, SegmentTransform &transform);
	shared_ptr<Segment> createSegment(shared_ptr<Segment> parent, float &radius, float &length, quat &rotation);
	
	void generateLeaves();
	shared_ptr<Mesh> generateLeaf();

	//Leaves generation
	void generateInstancedLeaves();
	shared_ptr<InstancedMesh> createInstancedLeavesCubeMesh(vector<InstancedTransform> &instancedTransforms);
	shared_ptr<InstancedMesh> createInstancedLeavesPanelMesh(vector<InstancedTransform> &instancedTransforms);
	
	//Stuff
	float toRadians(float angle) {
		return angle * 3.14159265359 / 180;
	}

	float toAngle(float radians) {
		return radians * 180 / 3.14159265359;
	}
};

#endif