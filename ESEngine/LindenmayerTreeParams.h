#pragma once

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;

class Rule {
public:
	Rule(string symbol, string replacement, float probability = 1.0f, int allowedDepth = 0) : symbol(symbol), replacement(replacement), probability(probability), allowedDepth(allowedDepth) { };
	string symbol;
	int allowedDepth;
	string replacement;
	float probability;
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

	void addRule(Rule &rule);
	vector<Rule> getRules(string &symbol);
	double radians(double angle);

protected:
	map<string, vector<Rule>> rules;
	vector<string> split(string lane);
};