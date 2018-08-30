#include "LindenmayerTreeSolver.h"

boost::variate_generator<boost::mt19937, boost::uniform_real<> > LindenmayerTreeSolver::randomGenerator(boost::mt19937(time(0)), boost::uniform_real<>(0, 1));

string LindenmayerTreeSolver::generateTreeProduction(LindenmayerTreeParams &params) {
	string product = params.axiom;

	string regexString = generateSymbolRegex(params.getAvailableRules());

	for (int i = 0; i < params.depth; i++) {
		regex regex(regexString);
		smatch res;
		string::const_iterator searchStart(product.cbegin());
		
		string newProduct;
		while (regex_search(searchStart, product.cend(), res, regex)) {
			searchStart += res.position() + res.length();
			
			string found = res[0];
			newProduct += res.prefix();
			newProduct += parseRule(params, found, i);
		}
		newProduct += res.suffix();
		if(newProduct.length() > 0)
			product = newProduct;
	}

	return product;
}

string LindenmayerTreeSolver::parseRule(LindenmayerTreeParams &params, string& symbol, int &depth) {
	vector<Rule> rules;

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

string LindenmayerTreeSolver::applyRule(string symbol, Rule rule) {
	map<string, string> parameters;
	
	if (symbol.length() > 1 && symbol[1] == '(') {
		vector<string> values = split(symbol.substr(2, symbol.length() - 3), ",");
		for (int i = 0; i < rule.parameters.size(); i++)
			parameters[rule.parameters[i]] = values[i];
	}

	string production = fillRuleWithParameters(rule.production, parameters);

	regex valueRegex("[(,]{1}[\\-0-9\\.]+(\\*|\\+|-|/){1}[\\*\\+/\\-0-9\\.]+");
	smatch valueRes;
	string::const_iterator paramSearchStart(production.cbegin());

	regex mathSignRegex("(\\*|\\+|-|/)");
	smatch mathSignRes;

	int paramIndex = 0;

	while (regex_search(paramSearchStart, production.cend(), valueRes, valueRegex))
	{
		string hit = valueRes[0].str();
		string found = valueRes[0].str().substr(1, valueRes[0].str().length() - 1);

		float value = 0;

		if (regex_search(found.cbegin(), found.cend(), mathSignRes, mathSignRegex)) {
			char c = mathSignRes.str()[0];
			string leftSide = mathSignRes.prefix().str();
			string rightSide = mathSignRes.suffix().str();

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
			production = production.substr(0, replacementIndex) + to_string(value) + production.substr(lengthOfReplacement);
			paramSearchStart = production.cbegin();
		}
		else
			return production;
	}
	return production;
}

string LindenmayerTreeSolver::fillRuleWithParameters(string rule, map<string, string> parameters) {
	string newRule = rule;

	pair<string, string> pair;
	BOOST_FOREACH(pair, parameters) {
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
			temporaryStr += pair.second;
			temporaryStr += found[found.length() - 1];
		}
		temporaryStr += res.suffix();

		if (temporaryStr.length() > 0)
			newRule = temporaryStr;
	}
	return newRule;
}

string LindenmayerTreeSolver::generateSymbolRegex(vector<string> symbols) {
	string regexString("(");
	BOOST_FOREACH(string symbol, symbols) {
		regexString += symbol + "|";
	}
	regexString = regexString.substr(0, regexString.length() - 1) + "){1}([(]{1}[0-9,\\.\\+\\-\\*/]*[)]{1})?";

	return regexString;
}