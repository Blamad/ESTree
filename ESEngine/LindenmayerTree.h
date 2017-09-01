#pragma once
#include <string>
#include <stack>
#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"
#include <time.h>

#include "Tree.h"
#include "InstancedMesh.h"
#include "LindenmayerTreeParams.h"

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

	void generateTexturedTree();
	float getNumericParameter(string product, int index);
	float returnNewIndexAfterParameter(string product, int index);
	float toRadians(float angle);

	void generateProduct();
	string parseRule(string &symbol, int &depth);
	void generateTexturedMesh();

	void generateInstancedLeaves();
	shared_ptr<InstancedMesh> createInstancedLeavesCubeMesh(vector<InstancedTransform> &instancedTransforms);
	shared_ptr<InstancedMesh> createInstancedLeavesPanelMesh(vector<InstancedTransform> &instancedTransforms);
};