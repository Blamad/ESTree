#pragma once

#include <vector>
#include <map>
#include <string>
#include <regex>
#include <fstream>
#include <iostream>

#include <GLM\common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <boost/random.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

using namespace std;
using namespace glm;
using namespace rapidjson;

class Rule {
public:
	Rule(string symbol, string replacement, float probability = 1.0f, int allowedDepth = 0, vector<string> variables = {}) : symbol(symbol), production(replacement), probability(probability), allowedDepth(allowedDepth), parameters(variables) { };
	string symbol;
	int allowedDepth;
	string production;
	float probability;
	vector<string> parameters;
};

class LindenmayerTreeParams {
public:
	LindenmayerTreeParams(string axiom, int depth, float initialLength = 1.0f, float initialRadius = 1.0f, float defaultAngle = 15.0f, float leafGrowthProb = 1.0)
		: axiom(axiom), depth(depth), initialLength(initialLength), initialRadius(initialRadius), angle(radians(defaultAngle)), leafGrowthProb(leafGrowthProb) { };

	LindenmayerTreeParams(string filePath);

	string filepath;
	string name;

	string axiom;
	int depth;
	float initialLength = 1.0f;
	float initialRadius = 1.0f;
	float angle = 15.0f;
	float leafGrowthProb = 1.0f;
	float leavesAngleDiversity = 0.0f;
	vec3 tropism = vec3(0,-1,0);
	float tropismBendingFactor = 0.2f;

	map<string, float> customParameters;

	void addRule(Rule &rule);
	vector<Rule> getRules(string &symbol);
	vector<string> getAvailableRules();
	double radians(double angle);

protected:
	map<string, vector<Rule>> rules;
	

	void loadFile(string &filepath);
	void readLFile(string filePath);
	void readJSONFile(string filePath);
	
	vector<string> split(string lane);
	vector<string> split(string lane, string splitter);
	string fillRuleWithVariables(string rule, map<string, float> variables);
};