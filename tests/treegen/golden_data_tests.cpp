// Checks of the golden data itself and of the properties of the shipped rule files that used to live only in the
// legacy characterization test (deleted in T07 together with the legacy pipeline): the golden directories are
// complete and have no stale files, deterministic rule files do not depend on the seed, stochastic rule files stay in
// their alphabet with balanced brackets. The golden contents are compared by loader_golden_tests.cpp and
// rewriter_golden_tests.cpp.
#include <doctest/doctest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <treegen/Module.h>
#include <treegen/Rewriter.h>
#include <treegen/TreeParams.h>

namespace fs = std::filesystem;

namespace {

const fs::path kRepoRoot = fs::path(ESTREE_REPO_ROOT);
const fs::path kRulesDir = kRepoRoot / "Resources" / "LindenmayerRules";
const fs::path kGoldenDir = kRepoRoot / "tests" / "golden";

struct RuleFile {
	std::string id;   // "advancedTree.json" or "legacy__advancedTree.l"
	fs::path path;
	bool legacyL;
};

std::vector<RuleFile> shippedRuleFiles() {
	std::vector<RuleFile> files;
	for (auto& e : fs::directory_iterator(kRulesDir))
		if (e.is_regular_file() && e.path().extension() == ".json")
			files.push_back({ e.path().filename().string(), e.path(), false });
	for (auto& e : fs::directory_iterator(kRulesDir / "legacy"))
		if (e.is_regular_file() && e.path().extension() == ".l")
			files.push_back({ "legacy__" + e.path().filename().string(), e.path(), true });
	std::sort(files.begin(), files.end(), [](const RuleFile& a, const RuleFile& b) { return a.id < b.id; });
	return files;
}

std::string readFile(const fs::path& p) {
	std::ifstream in(p, std::ios::binary);
	std::stringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

// A .l file sets the depth with `define depth` or with a bare number as its first line; otherwise the depth is a
// default and the solver goldens do not include "its own depth".
bool definesDepth(const RuleFile& f) {
	if (!f.legacyL)
		return true;
	std::istringstream in(readFile(f.path));
	std::string line;
	bool first = true;
	while (std::getline(in, line)) {
		if (line.compare(0, 13, "define depth ") == 0)
			return true;
		if (first && !line.empty() && line[0] != '#') {
			first = false;
			if (line.find_first_not_of("0123456789.") == std::string::npos)
				return true;
		}
	}
	return false;
}

// Deterministic = every symbol has exactly one production and it is always chosen (probability 1).
bool isDeterministic(const treegen::TreeParams& p) {
	for (const auto& entry : p.rules)
		if (entry.second.size() != 1 || entry.second[0].probability != 1.0f)
			return false;
	return true;
}

bool bracketsBalanced(const std::string& s) {
	int level = 0;
	for (char c : s) {
		if (c == '[')
			++level;
		if (c == ']' && --level < 0)
			return false;
	}
	return level == 0;
}

}

TEST_CASE("Golden data: the params and solver goldens are complete and have no stale files") {
	std::set<std::string> expectedParams;
	std::set<std::string> expectedSolver;
	size_t deterministicFiles = 0;
	for (const RuleFile& f : shippedRuleFiles()) {
		expectedParams.insert(f.id + ".txt");
		const treegen::TreeParams p = treegen::loadTreeParams(f.path);
		if (!isDeterministic(p))
			continue;
		++deterministicFiles;
		std::set<int> depths = { 0, 1, 2, 3 };
		if (definesDepth(f))
			depths.insert(p.depth);
		for (int d : depths)
			expectedSolver.insert(f.id + ".d" + std::to_string(d) + ".txt");
	}
	CHECK(deterministicFiles == 34);

	std::set<std::string> haveParams;
	for (const auto& e : fs::directory_iterator(kGoldenDir / "params"))
		haveParams.insert(e.path().filename().string());
	for (const std::string& name : haveParams)
		CHECK_MESSAGE(expectedParams.count(name) == 1, "stale params golden " << name);
	for (const std::string& name : expectedParams)
		CHECK_MESSAGE(haveParams.count(name) == 1, "missing params golden " << name);

	std::set<std::string> haveSolver;
	for (const auto& e : fs::directory_iterator(kGoldenDir / "solver"))
		if (e.path().extension() == ".txt")
			haveSolver.insert(e.path().filename().string());
	for (const std::string& name : haveSolver)
		CHECK_MESSAGE(expectedSolver.count(name) == 1, "stale solver golden " << name);
	for (const std::string& name : expectedSolver)
		CHECK_MESSAGE(haveSolver.count(name) == 1, "missing solver golden " << name);
}

TEST_CASE("Golden data: deterministic rule files give the same production for every seed") {
	// Probability 1 always wins, so the draw the rewriter makes per module cannot change the outcome.
	for (const RuleFile& f : shippedRuleFiles()) {
		const treegen::TreeParams probe = treegen::loadTreeParams(f.path);
		if (!isDeterministic(probe))
			continue;
		CAPTURE(f.id);
		treegen::TreeParams p = probe;
		p.depth = 2;
		treegen::RewriteOptions options;
		options.seed = 1;
		const std::string first = treegen::toString(treegen::rewrite(p, options));
		for (uint32_t seed = 2; seed <= 5; ++seed) {
			options.seed = seed;
			CHECK(treegen::toString(treegen::rewrite(p, options)) == first);
		}
	}
}

TEST_CASE("Golden data: stochastic rule files keep the alphabet and balanced brackets") {
	size_t stochasticFiles = 0;
	for (const RuleFile& f : shippedRuleFiles()) {
		treegen::TreeParams p = treegen::loadTreeParams(f.path);
		if (isDeterministic(p))
			continue;
		CAPTURE(f.id);
		++stochasticFiles;

		// alphabet: interpreter symbols + number syntax + every character of the axiom and of all productions
		std::set<char> allowed;
		for (char c : std::string("Ffv^<>+-;`=$T[]X~(),.*/0123456789"))
			allowed.insert(c);
		for (char c : p.axiom)
			allowed.insert(c);
		for (const auto& entry : p.rules)
			for (const treegen::Rule& rule : entry.second) {
				for (char c : rule.production)
					allowed.insert(c);
				allowed.insert(rule.symbol);
			}

		p.depth = definesDepth(f) ? p.depth : 3;
		for (uint32_t seed = 1; seed <= 5; ++seed) {
			treegen::RewriteOptions options;
			options.seed = seed;
			const std::string product = treegen::toString(treegen::rewrite(p, options));
			CHECK(product.size() > 0);
			CHECK(bracketsBalanced(product));
			const bool alphabetOk = std::all_of(product.begin(), product.end(), [&allowed](char c) { return allowed.count(c) == 1; });
			CHECK_MESSAGE(alphabetOk, "unexpected character in the product of " << f.id);
		}
	}
	CHECK(stochasticFiles == 9);
}
