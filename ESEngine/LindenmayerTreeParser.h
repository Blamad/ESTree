#ifndef LINDENMAYER_TREE_PARSER_H
#define LINDENMAYER_TREE_PARSER_H

#include <string>

#include <boost/random.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "LindenmayerTreeParams.h"

using namespace std;

class LindenmayerTreeParser {
public:
	LindenmayerTreeParser(LindenmayerTreeParams &params) : params(params) { }
	string generateTreeProduction();

private:
	LindenmayerTreeParams params;
	static boost::variate_generator<boost::mt19937, boost::uniform_real<>> randomGenerator;	
	
	string parseRule(string& symbol, int &depth);
	string fillRuleWithVariables(string rule, map<string, float> variables);
	string applyRule(string parent, Rule rule);
};

#endif