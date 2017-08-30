#pragma once
#include <string>
#include <stack>
#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"
#include <time.h>

#include "Tree.h"
#include "InstancedMesh.h"
#include "LindenmayerTreeParams.h"

class SegmentTransform {
public:
	SegmentTransform(quat &rotation, float length, float radius) : rotation(rotation), length(length), radius(radius) {};
	quat rotation;
	float length;
	float radius;
};

class LindenmayerTree : public Tree {
public: 
	LindenmayerTree(LindenmayerTreeParams &params, Material &material = Material::copper(), Material &leavesMaterial = Material::leaves2());

	void generate();

private:
	LindenmayerTreeParams params;
	string product;
	vector<shared_ptr<Segment>> segmentsVec;
	Material leavesMaterial;
	
	static boost::variate_generator<boost::mt19937, boost::uniform_real<>> randomGenerator;

	void generateTree();
	void generateTexturedTree();

	void generateProduct();
	string parseRule(string &symbol);
	void generateMesh();
	void generateTexturedMesh();

	void generateInstancedLeaves();
	shared_ptr<InstancedMesh> createInstancedLeavesCubeMesh(vector<InstancedTransform> &instancedTransforms);
	shared_ptr<InstancedMesh> createInstancedLeavesPanelMesh(vector<InstancedTransform> &instancedTransforms);
};