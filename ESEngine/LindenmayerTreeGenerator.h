#ifndef LINDENMAYER_TREE_GENERATOR_H
#define LINDENMAYER_TREE_GENERATOR_H

#include <string>

#include <boost/random.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "LindenmayerTreeParams.h"

using namespace std;

class LindenmayerTreeGenerator {
public:
	LindenmayerTreeGenerator(LindenmayerTreeParams &params) : params(params) { }
	string generateTreeProduction();

private:
	LindenmayerTreeParams params;
	static boost::variate_generator<boost::mt19937, boost::uniform_real<>> randomGenerator;	
	
	string parseRule(string& symbol, int &depth);
	string applyRule(string parent, Rule rule);
	string fillRuleWithParameters(string rule, map<string, string> parameters);
	string generateSymbolRegex(vector<string> symbols);

	vector<string> split(string lane, string splitter) {
		vector<string> splitLane;
		boost::split(splitLane, lane, boost::is_any_of(splitter));
		return splitLane;
	}
};

#endif