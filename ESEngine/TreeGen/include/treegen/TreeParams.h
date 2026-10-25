#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include <glm/glm.hpp>

namespace treegen {

struct Rule {
	char symbol;                              // single character
	std::vector<std::string> paramNames;      // formal parameters, e.g. {"l","w"}
	std::string production;                   // production text, custom parameters already substituted
	float probability = 1.0f;
	int   initialDepth = 0;                   // rule applies when iteration index >= initialDepth
};

struct TreeParams {
	std::string name;                         // source path / label
	std::string axiom;
	int   depth = 0;
	float initialLength = 1.0f, initialRadius = 1.0f;
	float angle = glm::radians(15.0f);        // radians
	float leafGrowthProb = 1.0f;
	float leavesAngleDiversity = 0.0f;        // radians
	glm::vec3 tropism{0, -1, 0};              // normalized when loaded
	float tropismBendingFactor = 0.2f;
	std::map<std::string, float> customParameters;
	std::map<char, std::vector<Rule>> rules;  // declaration order preserved per symbol
	uint32_t seed = 0;                        // optional "seed" key; 0 = caller decides
	std::size_t maxModules = 5'000'000;       // optional "maxModules" key
};

// .json or .l (by extension, case-insensitive; anything else is read as JSON). Throws LoadError whose message
// starts with the path (and the line for .l files) and states the reason.
TreeParams loadTreeParams(const std::filesystem::path& file);

// JSON text; `name` is only used for TreeParams::name and in error messages.
TreeParams parseTreeParamsJson(std::string_view json, std::string name = {});

// Text of a legacy `.l` file, same rules as loadTreeParams.
TreeParams parseTreeParamsLegacy(std::string_view text, std::string name = {});

// Replaces every whole-word occurrence of each parameter name by std::to_string(value). A word is bounded by
// anything but [A-Za-z0-9] or by the ends of the text. Parameters are applied in map order, one after another.
std::string substituteParameters(std::string_view text, const std::map<std::string, float>& values);

}
