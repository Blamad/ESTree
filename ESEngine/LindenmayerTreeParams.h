#pragma once

#include <vector>
#include <map>
#include <string>
#include <regex>
#include <fstream>
#include <iostream>

#include <boost/random.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;

class Rule {
public:
	Rule(string symbol, string replacement, float probability = 1.0f, int allowedDepth = 0, vector<string> variables = {}) : symbol(symbol), replacement(replacement), probability(probability), allowedDepth(allowedDepth), variables(variables) { };
	string symbol;
	int allowedDepth;
	string replacement;
	float probability;
	vector<string> variables;
};

class LindenmayerTreeParams {
public:
	LindenmayerTreeParams(string axiom, int depth, float initialLength = 1.0f, float initialRadius = 1.0f, float defaultAngle = 15.0f)
		: axiom(axiom), depth(depth), initialLength(initialLength), initialRadius(initialRadius), angle(radians(defaultAngle)) { };

	LindenmayerTreeParams(string filePath);

	string name;

	string axiom;
	int depth;
	float initialLength;
	float initialRadius;
	float angle;

	map<string, float> customParameters;

	void addRule(Rule &rule);
	vector<Rule> getRules(string &symbol);
	vector<string> getAvailableRules();
	double radians(double angle);

protected:
	map<string, vector<Rule>> rules;
	vector<string> split(string lane);
	vector<string> split(string lane, string splitter);
	string fillRuleWithVariables(string rule, map<string, float> variables);
	string applyRule(string parent, Rule rule);
};