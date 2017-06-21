#pragma once

#include <vector>
#include <map>
#include <string>

using namespace std;

class Rule {
public:
	Rule(string symbol, string replacement, float probability = 1.0f) : symbol(symbol), replacement(replacement), probability(probability) { };
	string symbol;
	string replacement;
	float probability;
};

class LindenmayerTreeParams {
public:
	LindenmayerTreeParams(string axiom, int depth, float initialLength = 1.0f, float initialRadius = 1.0f, float defaultAngle = 15.0f)
		: axiom(axiom), depth(depth), initialLength(initialLength), initialRadius(initialRadius), angle(radians(defaultAngle)) { };

	string axiom;
	int depth;
	float initialLength;
	float initialRadius;
	float angle;

	void addRule(Rule &rule) {
		rules[rule.symbol].push_back(rule);
	}

	vector<Rule> getRules(string &symbol) {
		return rules[symbol];
	}

protected:
	map<string, vector<Rule>> rules;
};