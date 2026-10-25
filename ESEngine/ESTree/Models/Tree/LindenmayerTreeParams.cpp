#include "LindenmayerTreeParams.h"

#include <stdexcept>

LindenmayerTreeParams::LindenmayerTreeParams(std::string axiomText, int treeDepth, float lengthValue, float radiusValue, float defaultAngle, float leafProbability) {
	axiom = axiomText;
	depth = treeDepth;
	initialLength = lengthValue;
	initialRadius = radiusValue;
	angle = static_cast<float>(radians(defaultAngle));
	leafGrowthProb = leafProbability;
}

LindenmayerTreeParams::LindenmayerTreeParams(std::string path)
	: treegen::TreeParams(treegen::loadTreeParams(path)), filepath(path) {
}

void LindenmayerTreeParams::addRule(const Rule &rule) {
	if (rule.symbol.size() != 1)
		throw std::invalid_argument("rule symbol must be a single character: \"" + rule.symbol + "\"");

	treegen::Rule converted;
	converted.symbol = rule.symbol[0];
	converted.paramNames = rule.parameters;
	converted.production = rule.production;
	converted.probability = rule.probability;
	converted.initialDepth = rule.allowedDepth;
	rules[converted.symbol].push_back(std::move(converted));
}

std::vector<Rule> LindenmayerTreeParams::getRules(const std::string &symbol) {
	std::vector<Rule> result;
	if (symbol.size() != 1)
		return result;
	auto found = rules.find(symbol[0]);
	if (found == rules.end())
		return result;
	for (const treegen::Rule &rule : found->second)
		result.push_back(Rule(std::string(1, rule.symbol), rule.production, rule.probability, rule.initialDepth, rule.paramNames));
	return result;
}

std::vector<std::string> LindenmayerTreeParams::getAvailableRules() {
	std::vector<std::string> symbols;
	for (const auto &entry : rules)
		symbols.push_back(std::string(1, entry.first));
	return symbols;
}

double LindenmayerTreeParams::radians(double degrees) {
	return (degrees * 3.14159265359) / 180;
}

std::string LindenmayerTreeParams::fillRuleWithVariables(std::string rule, std::map<std::string, float> variables) {
	return treegen::substituteParameters(rule, variables);
}
