#include "LindenmayerTreeParams.h"

LindenmayerTreeParams::LindenmayerTreeParams(string filePath) {
	ifstream lindenFile;
	lindenFile.exceptions(ifstream::badbit);
	int parsedLineCounter = 0;
	try
	{
		lindenFile.open(filePath);
		string lane;

		for (; getline(lindenFile, lane);) {
			//Comments
			if (lane[0] == '#')
				continue;

			switch (parsedLineCounter) {
			case 0:
				depth = atof(lane.c_str());
				break;
			case 1:
				initialLength = atof(lane.c_str());
				break;
			case 2:
				initialRadius = atof(lane.c_str());
				break;
			case 3:
				angle = atof(lane.c_str());
				angle = radians(angle);
				break;
			case 4:
				axiom = lane;
				break;
			default:
				vector<string> splitLane = split(lane);

				string symbol = splitLane[0];
				int allowedDepth = 0;
				if (symbol.length() > 1) {
					allowedDepth = atoi(symbol.substr(2, symbol.length() - 2).c_str());
					symbol = symbol.substr(0, 1);
				}

				string replacement = splitLane[1];
				double probability = 1.0;

				if (splitLane.size() == 3)
					probability = atof(splitLane[2].c_str());

				addRule(Rule(symbol, replacement, probability, allowedDepth));
			}

			parsedLineCounter++;
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
	vector<string> splitLane;
	boost::split(splitLane, lane, boost::is_any_of(" "));
	return splitLane;
}