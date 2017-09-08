#include "LindenmayerTreeParser.h"

boost::variate_generator<boost::mt19937, boost::uniform_real<> > LindenmayerTreeParser::randomGenerator(boost::mt19937(time(0)), boost::uniform_real<>(0, 1));

string LindenmayerTreeParser::generateTreeProduction() {
	string product = params.axiom;

	string regexString("(");
	BOOST_FOREACH(string val, params.getAvailableRules()) {
		regexString += val + "|";
	}
	regexString = regexString.substr(0, regexString.length() - 1) + "){1}([(]{1}[0-9,\\.\\+\\-\\*/]*[)]{1})?";

	for (int i = 0; i < params.depth; i++) {
		regex regex(regexString);
		smatch res;
		string::const_iterator searchStart(product.cbegin());
		
		string newProduct;
		while (regex_search(searchStart, product.cend(), res, regex))
		{
			searchStart += res.position() + res.length();
			
			string found = res[0];
			newProduct += res.prefix();
			newProduct += parseRule(found, i);
		}
		newProduct += res.suffix();
		if(newProduct.length() > 0)
			product = newProduct;
	}

	return product;
}

string LindenmayerTreeParser::parseRule(string& symbol, int &depth) {
	vector<Rule> rules;
	float randomSpace = 0;

	BOOST_FOREACH(Rule rule, params.getRules(symbol.substr(0,1))) {
		if (rule.allowedDepth <= depth) {
			rules.push_back(rule);
			randomSpace += rule.probability;
		}
	}

	//Only one rule
	if (rules.size() == 1)
		return applyRule(symbol, rules[0]);
	//There goes some random stuff
	if (rules.size() > 1) {
		float value = randomGenerator() * randomSpace;
		for (int i = 0; i < rules.size(); i++) {
			value -= rules[i].probability;
			if (value <= 0)
				return applyRule(symbol, rules[i]);
		}
	}
	//No rules available
	return symbol;
}

string LindenmayerTreeParser::fillRuleWithVariables(string rule, map<string, float> variables) {
	string newRule = rule;

	pair<string, float> pair;
	BOOST_FOREACH(pair, variables) {
		regex regex("[^a-zA-Z0-9]{1}" + pair.first + "[^a-zA-Z0-9]{1}");
		smatch res;
		string::const_iterator searchStart(newRule.cbegin());

		string temporaryStr;

		while (regex_search(searchStart, newRule.cend(), res, regex))
		{
			string found = res[0];
			searchStart += res.position() + res.length();

			temporaryStr += res.prefix();
			temporaryStr += found[0];
			temporaryStr += to_string(pair.second);
			temporaryStr += found[found.length() - 1];
			
		}
		temporaryStr += res.suffix();
		if (temporaryStr.length() > 0)
			newRule = temporaryStr;
	}
	return newRule;
}

string LindenmayerTreeParser::applyRule(string parent, Rule rule) {
	map<string, float> variables;

	regex paramRegex("[(,]{1}[\\*\\+/\\-0-9\\.]*");
	smatch res;
	string::const_iterator paramSearchStart(parent.cbegin());

	regex innerMathRegex("[0-9]{1}[\\*\\+\\-/]");
	smatch innerMathRes;

	int paramIndex = 0;

	while (regex_search(paramSearchStart, parent.cend(), res, paramRegex))
	{
		paramSearchStart += res.position() + res.length();
		string found = res[0].str().substr(1, res[0].str().length() - 1);

		float value = 0;

		if (regex_search(found.cbegin(), found.cend(), innerMathRes, innerMathRegex)) {
			char c = innerMathRes.str()[1];

			switch (c) {
			case '+':
				value = atof(innerMathRes.prefix().str().c_str()) + atof(innerMathRes.suffix().str().c_str());
				break;
			case '-':
				value = atof(innerMathRes.prefix().str().c_str()) - atof(innerMathRes.suffix().str().c_str());
				break;
			case '*':
				value = atof(innerMathRes.prefix().str().c_str()) * atof(innerMathRes.suffix().str().c_str());
				break;
			case '/':
				value = atof(innerMathRes.prefix().str().c_str()) / atof(innerMathRes.suffix().str().c_str());
				break;
			}
		}
		else {
			value = atof(found.c_str());
		}
		variables[rule.variables[paramIndex++]] = value;
	}

	return fillRuleWithVariables(rule.replacement, variables);
}