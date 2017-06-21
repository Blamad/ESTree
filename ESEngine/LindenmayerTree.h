#pragma once
#include <string>
#include <stack>
#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"
#include <time.h>

#include "Tree.h"
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
	LindenmayerTree(LindenmayerTreeParams &params);

private:
	LindenmayerTreeParams params;
	string product;
	
	static boost::variate_generator<boost::mt19937, boost::uniform_real<>> randomGenerator;

	void generateTree();

	void generateProduct();
	string parseRule(string &symbol);
	void generateMesh();
};