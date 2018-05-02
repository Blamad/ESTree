#ifndef CONSOLE_MEMORY_H
#define CONSOLE_MEMORY_H

#include <vector>
#include <string>

using namespace std;

class ConsoleMemory {
public:
	ConsoleMemory() {
		resetInputHistoryIndex();
	}

	void clear() { lines.clear(); }
	void pushLine(string line) { lines.push_back(line); }
	void resetInputHistoryIndex() { lastInputIndex = 0;  }
	vector<string> getLines() { return lines; }

	void pushInputLine(string line) {
		resetInputHistoryIndex();
		lines.push_back(line);
		inputHistory.push_back(line);
	}
	
	string getPreviousInput() {
		if (inputHistory.empty())
			return "";

		lastInputIndex--;
		if (lastInputIndex < 0)
			lastInputIndex = inputHistory.size() - 1;
		return inputHistory[lastInputIndex];
	}

	string getNextInput() {
		if (inputHistory.empty())
			return "";
		
		lastInputIndex++;
		if (lastInputIndex > inputHistory.size() - 1)
			lastInputIndex = 0;
		return inputHistory[lastInputIndex];
	}

private:
	vector<string> lines;
	
	vector<string> inputHistory;
	int lastInputIndex;

};

#endif