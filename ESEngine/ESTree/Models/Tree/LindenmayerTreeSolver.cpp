#include "LindenmayerTreeSolver.h"

boost::variate_generator<boost::mt19937, boost::uniform_real<> > LindenmayerTreeSolver::randomGenerator(boost::mt19937(time(0)), boost::uniform_real<>(0, 1));

std::string LindenmayerTreeSolver::generateTreeProduction(LindenmayerTreeParams &params) {
	std::string product = params.axiom;

	std::string regexString = generateSymbolRegex(params.getAvailableRules());

	for (int i = 0; i < params.depth; i++) {
		std::regex regex(regexString);
		std::smatch res;
		std::string::const_iterator searchStart(product.cbegin());
		
		std::string newProduct;
		while (regex_search(searchStart, product.cend(), res, regex)) {
			searchStart += res.position() + res.length();
			
			std::string found = res[0];
			newProduct += res.prefix();
			newProduct += parseRule(params, found, i);
		}
		newProduct += res.suffix();
		if(newProduct.length() > 0)
			product = newProduct;
	}

	return product;
}

std::string LindenmayerTreeSolver::parseRule(LindenmayerTreeParams &params, std::string& symbol, int &depth) {
	std::vector<Rule> rules;

	BOOST_FOREACH(Rule rule, params.getRules(symbol.substr(0,1))) {
		if (rule.allowedDepth <= depth) {
			rules.push_back(rule);
		}
	}

	//Pick rule
	float value = randomGenerator();
	for (int i = 0; i < rules.size(); i++) {
		value -= rules[i].probability;
		if (value <= 0)
			return applyRule(symbol, rules[i]);
	}

	//No rules available
	return symbol;
}

std::string LindenmayerTreeSolver::applyRule(std::string symbol, Rule rule) {
	std::map<std::string, std::string> parameters;
	
	if (symbol.length() > 1 && symbol[1] == '(') {
		std::vector<std::string> values = split(symbol.substr(2, symbol.length() - 3), ",");
		for (int i = 0; i < rule.parameters.size(); i++)
			parameters[rule.parameters[i]] = values[i];
	}

	std::string production = fillRuleWithParameters(rule.production, parameters);

	std::regex valueRegex("[(,]{1}[\\-0-9\\.]+(\\*|\\+|-|/){1}[\\*\\+/\\-0-9\\.]+");
	std::smatch valueRes;
	std::string::const_iterator paramSearchStart(production.cbegin());

	std::regex mathSignRegex("(\\*|\\+|-|/)");
	std::smatch mathSignRes;

	int paramIndex = 0;

	while (regex_search(paramSearchStart, production.cend(), valueRes, valueRegex))
	{
		std::string hit = valueRes[0].str();
		std::string found = valueRes[0].str().substr(1, valueRes[0].str().length() - 1);

		float value = 0;

		if (regex_search(found.cbegin(), found.cend(), mathSignRes, mathSignRegex)) {
			char c = mathSignRes.str()[0];
			std::string leftSide = mathSignRes.prefix().str();
			std::string rightSide = mathSignRes.suffix().str();

			switch (c) {
			case '*':
				value = atof(leftSide.c_str()) * atof(rightSide.c_str());
				break;
			case '/':
				value = atof(leftSide.c_str()) / atof(rightSide.c_str());
				break;
			case '+':
				value = atof(leftSide.c_str()) + atof(rightSide.c_str());
				break;
			case '-':
				value = atof(leftSide.c_str()) - atof(rightSide.c_str());
				break;
			}

			int replacementIndex = valueRes.position(0) + 1;
			int lengthOfReplacement = replacementIndex + valueRes.length(0) - 1;
			production = production.substr(0, replacementIndex) + std::to_string(value) + production.substr(lengthOfReplacement);
			paramSearchStart = production.cbegin();
		}
		else
			return production;
	}
	return production;
}

std::string LindenmayerTreeSolver::fillRuleWithParameters(std::string rule, std::map<std::string, std::string> parameters) {
	std::string newRule = rule;

	std::pair<std::string, std::string> pair;
	BOOST_FOREACH(pair, parameters) {
		std::regex regex("[^a-zA-Z0-9]{1}" + pair.first + "[^a-zA-Z0-9]{1}");
		std::smatch res;
		std::string::const_iterator searchStart(newRule.cbegin());

		std::string temporaryStr;

		while (regex_search(searchStart, newRule.cend(), res, regex))
		{
			std::string found = res[0];
			searchStart += res.position() + res.length();

			temporaryStr += res.prefix();
			temporaryStr += found[0];
			temporaryStr += pair.second;
			temporaryStr += found[found.length() - 1];
		}
		temporaryStr += res.suffix();

		if (temporaryStr.length() > 0)
			newRule = temporaryStr;
	}
	return newRule;
}

std::string LindenmayerTreeSolver::generateSymbolRegex(std::vector<std::string> symbols) {
	std::string regexString("(");
	BOOST_FOREACH(std::string symbol, symbols) {
		regexString += symbol + "|";
	}
	regexString = regexString.substr(0, regexString.length() - 1) + "){1}([(]{1}[0-9,\\.\\+\\-\\*/]*[)]{1})?";

	return regexString;
}