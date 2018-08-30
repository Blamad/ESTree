#include "LindenmayerTreeParams.h"

LindenmayerTreeParams::LindenmayerTreeParams(string filePath) {
	filepath = filePath;
	loadFile(filepath);
}

void LindenmayerTreeParams::loadFile(string &filepath) {
	if (filepath.substr(filepath.find_last_of('.') + 1) == "l")
		readLFile(filepath);
	else
		readJSONFile(filepath);
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
		
		if (paramName == "initialLength") {
			float paramValue = m.value.GetFloat();
			initialLength = paramValue;
		}
		else if (paramName == "depth") {
			float paramValue = m.value.GetFloat();
			depth = paramValue;
		}
		else if (paramName == "initialRadius") {
			float paramValue = m.value.GetFloat();
			initialRadius = paramValue;
		}
		else if (paramName == "angle") {
			float paramValue = m.value.GetFloat();
			angle = radians(paramValue);
		}
		else if (paramName == "leavesAngleDiversity") {
			float paramValue = m.value.GetFloat();
			leavesAngleDiversity = radians(paramValue);
		}
		else if (paramName == "tropism") {
			tropism = vec3(m.value.GetArray()[0].GetFloat(), m.value.GetArray()[1].GetFloat(), m.value.GetArray()[2].GetFloat());
			tropism = normalize(tropism);
		}
		else if (paramName == "tropismBendingFactor") {
			float paramValue = m.value.GetFloat();
			tropismBendingFactor = paramValue;
		}
		else if (paramName == "leavesGrowthProbability") {
			float paramValue = m.value.GetFloat();
			leafGrowthProb = paramValue;
		}
		else {
			float paramValue = m.value.GetFloat();
			customParameters[paramName] = paramValue;
		}
	}
	
	//RULES
	axiom = document["axiom"].GetString();
	for (auto& rule : document["rules"].GetArray()) {
		string symbol = rule.GetObject()["symbol"].GetString();
		string production = rule.GetObject()["production"].GetString();

		vector<string> parameters;
		int intialDepth = 0;
		float probability = 1.0;

		if (symbol.length() > 1 && symbol[1] == '(') {
			parameters = split(symbol.substr(2, symbol.length() - 3), ",");
			symbol = symbol.substr(0, 1);
		}

		if (rule.GetObject().HasMember("probability")) {
			probability = rule.GetObject()["probability"].GetFloat();
		}
		if (rule.GetObject().HasMember("initialDepth")) {
			intialDepth = rule.GetObject()["initialDepth"].GetInt();
		}

		production = fillRuleWithVariables(production, customParameters);
		addRule(Rule(symbol, production, probability, intialDepth, parameters));
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
				else if (splitLane[1] == "leavesAngleDiversity") {
					leavesAngleDiversity = radians(atof(splitLane[2].c_str()));
				}
				else if (splitLane[1] == "tropism") {
					//TODO parse vector
				}
				else if (splitLane[1] == "tropismBendingFactor") {
					tropismBendingFactor = atof(splitLane[2].c_str());
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

vector<string> LindenmayerTreeParams::getAvailableRules() {
	pair<string, vector<Rule>> pair;
	vector<string> v;
	BOOST_FOREACH(pair, rules) {
		v.push_back(pair.first);
	}
	return v;
}