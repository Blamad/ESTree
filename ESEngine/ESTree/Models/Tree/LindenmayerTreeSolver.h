#ifndef LINDENMAYER_TREE_SOLVER_H
#define LINDENMAYER_TREE_SOLVER_H

#include <string>

#include <boost/random.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "LindenmayerTreeParams.h"

class LindenmayerTreeSolver {
public:
	static std::string generateTreeProduction(LindenmayerTreeParams &params);

private:
	LindenmayerTreeParams params;
	static boost::variate_generator<boost::mt19937, boost::uniform_real<>> randomGenerator;	
	
	static std::string parseRule(LindenmayerTreeParams &params, std::string& symbol, int &depth);
	static std::string applyRule(std::string parent, Rule rule);
	static std::string fillRuleWithParameters(std::string rule, std::map<std::string, std::string> parameters);
	static std::string generateSymbolRegex(std::vector<std::string> symbols);

	static std::vector<std::string> split(std::string lane, std::string splitter) {
		std::vector<std::string> splitLane;
		boost::split(splitLane, lane, boost::is_any_of(splitter));
		return splitLane;
	}
};

#endif