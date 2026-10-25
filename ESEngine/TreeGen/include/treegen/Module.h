#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace treegen {

struct Module {                   // one symbol + its numeric parameters
	char symbol = 0;
	std::vector<float> params;
	bool randomized = false;      // symbol followed by '~' : first param is an upper bound
};
using ModuleString = std::vector<Module>;

// "F(2)T+(45)A" ; throws RewriteError on malformed text. Parameters are plain numbers, expressions are
// resolved by the rewriter before a string reaches this function.
ModuleString parseModules(std::string_view text);

// Canonical text: parameters via std::to_string(float), joined by ','.
std::string toString(const ModuleString& modules);

}
