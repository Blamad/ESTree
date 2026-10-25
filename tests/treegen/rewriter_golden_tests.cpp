// The solver goldens of tests/golden/solver were produced by the legacy regex solver (T02) for the deterministic
// rule files. treegen::rewrite has to reproduce them on every platform (the legacy solver itself only works with
// MSVC). The legacy string keeps literal numbers of a production as written ("+(45)") and prints evaluated ones
// with std::to_string ("+(45.000000)"); a ModuleString only knows numbers, so both sides are compared in the
// canonical form toString(parseModules(text)). That is exact: every digit of every parameter is compared.
#include <doctest/doctest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <treegen/Error.h>
#include <treegen/Module.h>
#include <treegen/Rewriter.h>
#include <treegen/TreeParams.h>

namespace fs = std::filesystem;

namespace {

const fs::path kRepoRoot = fs::path(ESTREE_REPO_ROOT);
const fs::path kRulesDir = kRepoRoot / "Resources" / "LindenmayerRules";
const fs::path kSolverGoldenDir = kRepoRoot / "tests" / "golden" / "solver";

struct SolverGolden {
	std::string ruleId;     // "legacy__test.l"
	int depth;
	fs::path file;
};

std::string readFile(const fs::path& p) {
	std::ifstream in(p, std::ios::binary);
	std::stringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

fs::path ruleFilePath(const std::string& ruleId) {
	const std::string legacy = "legacy__";
	if (ruleId.compare(0, legacy.size(), legacy) == 0)
		return kRulesDir / "legacy" / ruleId.substr(legacy.size());
	return kRulesDir / ruleId;
}

std::vector<SolverGolden> solverGoldens() {
	std::vector<SolverGolden> goldens;
	for (const auto& entry : fs::directory_iterator(kSolverGoldenDir)) {
		std::string name = entry.path().filename().string();
		if (name.size() < 4 || name.substr(name.size() - 4) != ".txt")
			continue;
		name.resize(name.size() - 4);
		size_t dot = name.rfind(".d");
		REQUIRE(dot != std::string::npos);
		goldens.push_back({ name.substr(0, dot), std::stoi(name.substr(dot + 2)), entry.path() });
	}
	std::sort(goldens.begin(), goldens.end(), [](const SolverGolden& a, const SolverGolden& b) {
		return a.ruleId != b.ruleId ? a.ruleId < b.ruleId : a.depth < b.depth;
	});
	return goldens;
}

std::string firstDifference(const std::string& expected, const std::string& actual) {
	size_t i = 0;
	while (i < expected.size() && i < actual.size() && expected[i] == actual[i])
		++i;
	auto excerpt = [&](const std::string& s) { return s.substr(i > 40 ? i - 40 : 0, 100); };
	return "expected length " + std::to_string(expected.size()) + ", actual length " + std::to_string(actual.size()) +
		", first difference at " + std::to_string(i) + "\n  expected: ..." + excerpt(expected) + "\n  actual:   ..." + excerpt(actual);
}

}

TEST_CASE("rewrite goldens: every deterministic rule file reproduces its solver goldens") {
	std::vector<SolverGolden> goldens = solverGoldens();
	CHECK(goldens.size() >= 34 * 4);

	std::map<std::string, int> filesSeen;
	for (const SolverGolden& golden : goldens) {
		CAPTURE(golden.ruleId);
		CAPTURE(golden.depth);
		++filesSeen[golden.ruleId];

		treegen::TreeParams params = treegen::loadTreeParams(ruleFilePath(golden.ruleId));
		params.depth = golden.depth;
		treegen::RewriteOptions options;
		options.seed = 7;

		std::string expectedText = readFile(golden.file);
		REQUIRE(expectedText.compare(0, 7, "length=") != 0); // hashed goldens are not used by any shipped file
		try {
			std::string expected = treegen::toString(treegen::parseModules(expectedText));
			std::string actual = treegen::toString(treegen::rewrite(params, options));
			if (expected != actual)
				FAIL_CHECK("golden mismatch in " << golden.file.filename().string() << ": " << firstDifference(expected, actual));
			else
				CHECK(true);
		}
		catch (const treegen::Error& e) {
			FAIL_CHECK(golden.file.filename().string() << ": " << std::string(e.what()));
		}
	}
	CHECK(filesSeen.size() == 34);
}

TEST_CASE("rewrite goldens: legacy/parametricTree2.l generates now that its lost parenthesis is back, the old text is a RewriteError") {
	// Until T07 the rule for A of this file contained "[^+a)": after substitution "[^+18.950001)", a ')' without '('.
	// The legacy solver copied the stray ')' into its output (D-014), the rewriter reports it, naming the rule.
	// T07 restored the '(' (D-017) and regenerated the solver goldens d1..d3 with treegen; the shipped file is checked by
	// the golden test above, the old text is rebuilt here to keep the error pinned on a real-world rule.
	const std::string fixedText = readFile(ruleFilePath("legacy__parametricTree2.l"));
	REQUIRE(fixedText.find("[^+(a)F(startLength)A]") != std::string::npos);
	treegen::TreeParams fixed = treegen::loadTreeParams(ruleFilePath("legacy__parametricTree2.l"));
	fixed.depth = 3;
	CHECK_NOTHROW(treegen::rewrite(fixed));

	std::string brokenText = fixedText;
	brokenText.replace(brokenText.find("[^+(a)"), 6, "[^+a)");
	treegen::TreeParams params = treegen::parseTreeParamsLegacy(brokenText, "parametricTree2.l (old text)");
	params.depth = 0;
	std::string axiom = readFile(kSolverGoldenDir / "legacy__parametricTree2.l.d0.txt");
	CHECK(treegen::toString(treegen::rewrite(params)) == treegen::toString(treegen::parseModules(axiom)));
	params.depth = 1;
	try {
		treegen::rewrite(params);
		FAIL("expected a RewriteError");
	}
	catch (const treegen::RewriteError& e) {
		std::string message = e.what();
		CHECK(message.find("rule A") != std::string::npos);
		CHECK(message.find("unexpected ')'") != std::string::npos);
	}
}
