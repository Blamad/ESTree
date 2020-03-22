#ifndef CONSOLE_MEMORY_H
#define CONSOLE_MEMORY_H

#include <vector>
#include <string>

class ConsoleMemory {
public:
	ConsoleMemory() {
		resetInputHistoryIndex();
	}

	void clear() { lines.clear(); }
	void pushLine(std::string line) { lines.push_back(line); }
	void resetInputHistoryIndex() { lastInputIndex = 0;  }
	std::vector<std::string> getLines() { return lines; }

	void pushInputLine(std::string line) {
		resetInputHistoryIndex();
		lines.push_back(line);
		inputHistory.push_back(line);
	}
	
	std::string getPreviousInput() {
		if (inputHistory.empty())
			return "";

		lastInputIndex--;
		if (lastInputIndex < 0)
			lastInputIndex = inputHistory.size() - 1;
		return inputHistory[lastInputIndex];
	}

	std::string getNextInput() {
		if (inputHistory.empty())
			return "";
		
		lastInputIndex++;
		if (lastInputIndex > inputHistory.size() - 1)
			lastInputIndex = 0;
		return inputHistory[lastInputIndex];
	}

private:
	std::vector<std::string> lines;
	
	std::vector<std::string> inputHistory;
	int lastInputIndex;

};

#endif