#ifndef LINDENMAYER_TREE_SOLVER_H
#define LINDENMAYER_TREE_SOLVER_H

#include <string>

#include <boost/random.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "LindenmayerTreeParams.h"

using namespace std;

class LindenmayerTreeSolver {
public:
	static string generateTreeProduction(LindenmayerTreeParams &params);

private:
	LindenmayerTreeParams params;
	static boost::variate_generator<boost::mt19937, boost::uniform_real<>> randomGenerator;	
	
	static string parseRule(LindenmayerTreeParams &params, string& symbol, int &depth);
	static string applyRule(string parent, Rule rule);
	static string fillRuleWithParameters(string rule, map<string, string> parameters);
	static string generateSymbolRegex(vector<string> symbols);

	static vector<string> split(string lane, string splitter) {
		vector<string> splitLane;
		boost::split(splitLane, lane, boost::is_any_of(splitter));
		return splitLane;
	}
};

#endif