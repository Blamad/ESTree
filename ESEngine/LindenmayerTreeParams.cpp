#include "LindenmayerTreeParams.h"

LindenmayerTreeParams::LindenmayerTreeParams(string filePath) {
	if(filePath.substr(filePath.find_last_of('.')+1) == "l")
		readLFile(filePath);
	else
		readJSONFile(filePath);
}

void LindenmayerTreeParams::readJSONFile(string filePath) {
	name = filePath;

	ifstream ifs(name);
	IStreamWrapper isw(ifs);
	Document document;
	document.ParseStream(isw);

	//PARAMETERS
	for (auto& m : document["parameters"].GetObject()) {
		string paramName = m.name.GetString(); 
		float paramValue = m.value.GetFloat();

		if (paramName == "initialLength") {
			initialLength = paramValue;
		}
		else if (paramName == "depth") {
			depth = paramValue;
		}
		else if (paramName == "initialRadius") {
			initialRadius = paramValue;
		}
		else if (paramName == "angle") {
			angle = radians(paramValue);
		}
		else {
			customParameters[paramName] = paramValue;
		}
	}
	
	//RULES
	axiom = document["axiom"].GetString();
	for (auto& rule : document["rules"].GetArray()) {
		string symbol = rule.GetObject()["symbol"].GetString();
		string production = rule.GetObject()["production"].GetString();

		vector<string> variables;
		int intialDepth = 0;
		float probability = 1.0;

		if (symbol.length() > 1 && symbol[1] == '(') {
			variables = split(symbol.substr(2, symbol.length() - 3), ",");
			symbol = symbol.substr(0, 1);
		}

		if (rule.GetObject().HasMember("probability")) {
			probability = rule.GetObject()["probability"].GetFloat();
		}
		if (rule.GetObject().HasMember("initialDepth")) {
			intialDepth = rule.GetObject()["initialDepth"].GetInt();
		}

		production = fillRuleWithVariables(production, customParameters);
		addRule(Rule(symbol, production, probability, intialDepth, variables));
	}
}

void LindenmayerTreeParams::readLFile(string filePath) {
	name = filePath;

	ifstream lindenFile;
	lindenFile.exceptions(ifstream::badbit);
	int parsedLineCounter = 0;
	try
	{
		lindenFile.open(filePath);
		string line;

		for (; getline(lindenFile, line);) {
			//Comments
			if (line.length() == 0 || line[0] == '#')
				continue;

			//Parameters
			if (line.length() > 6 && line.substr(0, 6) == "define") {
				vector<string> splitLane = split(line);

				if (splitLane[1] == "initialLength") {
					initialLength = atof(splitLane[2].c_str());
				}
				else if (splitLane[1] == "depth") {
					depth = atof(splitLane[2].c_str());
				}
				else if (splitLane[1] == "initialRadius") {
					initialRadius = atof(splitLane[2].c_str());
				}
				else if (splitLane[1] == "angle") {
					angle = radians(atof(splitLane[2].c_str()));
				}
				else {
					customParameters[splitLane[1]] = atof(splitLane[2].c_str());
				}
				continue;
			}

			//Axioms and rules
			vector<string> splitLine = split(line);
			if (splitLine.size() == 1) {
				axiom = splitLine[0];
				continue;
			}

			string symbol = splitLine[0];
			int allowedDepth = 0;
			vector<string> variables;

			if (symbol.length() > 1) {
				if (symbol[1] == '{') {
					allowedDepth = atoi(symbol.substr(2, symbol.length() - 2).c_str());
					symbol = symbol.substr(0, 1);
				}
				if (symbol[1] == '(') {
					variables = split(symbol.substr(2, symbol.length() - 3), ",");
					symbol = symbol.substr(0, 1);
				}
			}

			string replacement = splitLine[1];
			double probability = 1.0;

			if (splitLine.size() == 3)
				probability = atof(splitLine[2].c_str());

			replacement = fillRuleWithVariables(replacement, customParameters);

			addRule(Rule(symbol, replacement, probability, allowedDepth, variables));
		}
	}
	catch (ifstream::failure e)
	{
		cout << "ERROR LOADING LINDENMAYER TREE MODEL: " << filePath << endl;
	}
}

void LindenmayerTreeParams::addRule(Rule &rule) {
	rules[rule.symbol].push_back(rule);
}

vector<Rule> LindenmayerTreeParams::getRules(string &symbol) {
	return rules[symbol];
}

double LindenmayerTreeParams::radians(double angle) {
	return (angle * 3.14159265359) / 180;
}

vector<string> LindenmayerTreeParams::split(string lane) {
	return split(lane, " ");
}

vector<string> LindenmayerTreeParams::split(string lane, string splitter) {
	vector<string> splitLane;
	boost::split(splitLane, lane, boost::is_any_of(splitter));
	return splitLane;
}

string LindenmayerTreeParams::fillRuleWithVariables(string rule, map<string, float> variables) {
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
			temporaryStr += found[found.length()-1];
		}
		temporaryStr += res.suffix();

		if(temporaryStr.length() > 0)
			newRule = temporaryStr;
	}
	return newRule;
}

string LindenmayerTreeParams::applyRule(string parent, Rule rule) {
	map<string, float> variables;

	regex paramRegex("[(,]{1}[*+/-0-9.]*[,)]{1}");
	smatch res;
	string::const_iterator paramSearchStart(parent.cbegin());

	regex innerMathRegex("[0-9]{1}[*+-/]");
	smatch innerMathRes;

	int paramIndex = 0;

	while (regex_search(paramSearchStart, parent.cend(), res, paramRegex))
	{
		paramSearchStart += res.position() + res.length();
		string found = res[0].str().substr(1, res[0].str().length() - 2);

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
		} else {
			value = atof(found.c_str());
		}
		variables[rule.variables[paramIndex++]] = value;
	}

	pair<string, float> pair;
	BOOST_FOREACH(pair, variables) {
		variables[pair.first] = pair.second;
	}

	string result = fillRuleWithVariables(rule.replacement, variables);
	cout << rule.replacement << endl << result << endl;
	return result;
}

vector<string> LindenmayerTreeParams::getAvailableRules() {
	pair<string, vector<Rule>> pair;
	vector<string> v;
	BOOST_FOREACH(pair, rules) {
		v.push_back(pair.first);
	}
	return v;
}