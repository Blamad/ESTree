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

class Rule {
public:
	Rule(std::string symbol, std::string replacement, float probability = 1.0f, int allowedDepth = 0, std::vector<std::string> variables = {}) : symbol(symbol), production(replacement), probability(probability), allowedDepth(allowedDepth), parameters(variables) { };
	std::string symbol;
	int allowedDepth;
	std::string production;
	float probability;
	std::vector<std::string> parameters;
};

class LindenmayerTreeParams {
public:
	LindenmayerTreeParams(std::string axiom, int depth, float initialLength = 1.0f, float initialRadius = 1.0f, float defaultAngle = 15.0f, float leafGrowthProb = 1.0)
		: axiom(axiom), depth(depth), initialLength(initialLength), initialRadius(initialRadius), angle(radians(defaultAngle)), leafGrowthProb(leafGrowthProb) { };

	LindenmayerTreeParams(std::string filePath);

	std::string filepath;
	std::string name;

	std::string axiom;
	int depth;
	float initialLength = 1.0f;
	float initialRadius = 1.0f;
	float angle = 15.0f;
	float leafGrowthProb = 1.0f;
	float leavesAngleDiversity = 0.0f;
	glm::vec3 tropism = glm::vec3(0,-1,0);
	float tropismBendingFactor = 0.2f;

	std::map<std::string, float> customParameters;

	void addRule(Rule &rule);
	std::vector<Rule> getRules(std::string &symbol);
	std::vector<std::string> getAvailableRules();
	double radians(double angle);

protected:
	std::map<std::string, std::vector<Rule>> rules;
	

	void loadFile(std::string &filepath);
	void readLFile(std::string filePath);
	void readJSONFile(std::string filePath);
	
	std::vector<std::string> split(std::string lane);
	std::vector<std::string> split(std::string lane, std::string splitter);
	std::string fillRuleWithVariables(std::string rule, std::map<std::string, float> variables);
};