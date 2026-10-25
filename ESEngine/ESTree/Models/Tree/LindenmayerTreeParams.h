#pragma once

#include <vector>
#include <map>
#include <string>
#include <regex>
#include <fstream>
#include <iostream>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

// The includes below are no longer needed by this header itself; the other legacy Lindenmayer files
// (solver, interpreter, tree) still get them through it. They go away with those files (T07).
#include <boost/random.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <treegen/TreeParams.h>

class Rule {
public:
	Rule(std::string symbol, std::string replacement, float probability = 1.0f, int allowedDepth = 0, std::vector<std::string> variables = {}) : symbol(symbol), production(replacement), probability(probability), allowedDepth(allowedDepth), parameters(variables) { };
	std::string symbol;
	int allowedDepth;
	std::string production;
	float probability;
	std::vector<std::string> parameters;
};

// Adapter over treegen::TreeParams (loading, parsing and the data live there). The fields callers read
// (name, axiom, depth, initialLength, initialRadius, angle, leafGrowthProb, leavesAngleDiversity, tropism,
// tropismBendingFactor, customParameters) are inherited unchanged; Rule/addRule/getRules are a facade over
// treegen::Rule. Deleted together with the legacy pipeline in T07.
class LindenmayerTreeParams : public treegen::TreeParams {
public:
	LindenmayerTreeParams(std::string axiom, int depth, float initialLength = 1.0f, float initialRadius = 1.0f, float defaultAngle = 15.0f, float leafGrowthProb = 1.0f);

	// Throws treegen::LoadError when the file is missing or malformed.
	LindenmayerTreeParams(std::string filePath);

	std::string filepath;

	void addRule(const Rule &rule);
	std::vector<Rule> getRules(const std::string &symbol);
	std::vector<std::string> getAvailableRules();
	double radians(double angle);

protected:
	std::string fillRuleWithVariables(std::string rule, std::map<std::string, float> variables);
};
