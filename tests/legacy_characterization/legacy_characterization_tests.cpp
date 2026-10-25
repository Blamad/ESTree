// Characterization (golden) tests of the legacy Lindenmayer params loader and string rewriter.
// Compiled from the UNCHANGED legacy sources; documents what the code does today, including its bugs.
// Goldens: tests/golden/ (see tests/golden/README.md). Regenerate with `--regenerate`.
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "ESTree/Models/Tree/LindenmayerTreeParams.h"
#include "ESTree/Models/Tree/LindenmayerTreeSolver.h"

namespace fs = std::filesystem;

namespace {

bool g_regenerate = false;

const fs::path kRepoRoot = fs::path(ESTREE_REPO_ROOT);
const fs::path kRulesDir = kRepoRoot / "Resources" / "LindenmayerRules";
const fs::path kGoldenDir = kRepoRoot / "tests" / "golden";

// Solver goldens store the full string up to this length, otherwise length + FNV-1a hash.
constexpr size_t kMaxStoredLength = 200000;

// ---- helpers ------------------------------------------------------------------------------------

struct RuleFile {
	std::string id;   // "advancedTree.json" or "legacy__advancedTree.l" (used in golden file names)
	fs::path path;
	bool legacyL;
};

std::vector<RuleFile> shippedRuleFiles() {
	std::vector<RuleFile> files;
	for (auto& e : fs::directory_iterator(kRulesDir)) {
		if (e.is_regular_file() && e.path().extension() == ".json")
			files.push_back({ e.path().filename().string(), e.path(), false });
	}
	for (auto& e : fs::directory_iterator(kRulesDir / "legacy")) {
		if (e.is_regular_file() && e.path().extension() == ".l")
			files.push_back({ "legacy__" + e.path().filename().string(), e.path(), true });
	}
	std::sort(files.begin(), files.end(), [](const RuleFile& a, const RuleFile& b) { return a.id < b.id; });
	return files;
}

std::string readFile(const fs::path& p, bool* ok = nullptr) {
	std::ifstream in(p, std::ios::binary);
	if (ok) *ok = static_cast<bool>(in);
	std::stringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

void writeFile(const fs::path& p, const std::string& content) {
	fs::create_directories(p.parent_path());
	std::ofstream out(p, std::ios::binary | std::ios::trunc);
	out << content;
}

// Compares with a golden file; with --regenerate it rewrites the golden instead.
void checkGolden(const fs::path& goldenPath, const std::string& actual) {
	if (g_regenerate) {
		writeFile(goldenPath, actual);
		return;
	}
	bool ok = false;
	std::string expected = readFile(goldenPath, &ok);
	if (!ok) {
		FAIL_CHECK("missing golden file " << goldenPath.string() << " (run with --regenerate, review, commit)");
		return;
	}
	if (expected == actual) {
		CHECK(true);
		return;
	}
	size_t i = 0;
	while (i < expected.size() && i < actual.size() && expected[i] == actual[i]) ++i;
	auto excerpt = [&](const std::string& s) { return s.substr(i > 40 ? i - 40 : 0, 100); };
	FAIL_CHECK("golden mismatch in " << goldenPath.filename().string() << ": expected length " << expected.size()
		<< ", actual length " << actual.size() << ", first difference at " << i << "\n  expected: ..."
		<< excerpt(expected) << "\n  actual:   ..." << excerpt(actual));
}

std::string fmt(double v) {
	char buf[64];
	std::snprintf(buf, sizeof(buf), "%.9g", v);
	return buf;
}

std::string join(const std::vector<std::string>& v, const char* sep) {
	std::string s;
	for (size_t i = 0; i < v.size(); ++i) s += (i ? sep : "") + v[i];
	return s;
}

uint64_t fnv1a64(const std::string& s) {
	uint64_t h = 1469598103934665603ull;
	for (unsigned char c : s) { h ^= c; h *= 1099511628211ull; }
	return h;
}

// .l files that have no `define depth` line leave LindenmayerTreeParams::depth uninitialized (reading it is
// indeterminate), see KNOWN_BUGS.md. JSON files always define it.
bool definesDepth(const RuleFile& f) {
	if (!f.legacyL) return true;
	std::istringstream in(readFile(f.path));
	std::string line;
	while (std::getline(in, line))
		if (line.compare(0, 13, "define depth ") == 0) return true;
	return false;
}

std::string dumpParams(const RuleFile& f, LindenmayerTreeParams& p) {
	std::ostringstream o;
	o << "file=" << (f.legacyL ? "legacy/" : "") << f.path.filename().string() << "\n";
	o << "axiom=" << p.axiom << "\n";
	o << "depth=" << (definesDepth(f) ? std::to_string(p.depth) : std::string("UNSET")) << "\n";
	o << "initialLength=" << fmt(p.initialLength) << "\n";
	o << "initialRadius=" << fmt(p.initialRadius) << "\n";
	o << "angle=" << fmt(p.angle) << "\n";
	o << "leafGrowthProb=" << fmt(p.leafGrowthProb) << "\n";
	o << "leavesAngleDiversity=" << fmt(p.leavesAngleDiversity) << "\n";
	o << "tropism=" << fmt(p.tropism.x) << "," << fmt(p.tropism.y) << "," << fmt(p.tropism.z) << "\n";
	o << "tropismBendingFactor=" << fmt(p.tropismBendingFactor) << "\n";
	o << "customParameters.count=" << p.customParameters.size() << "\n";
	for (auto& kv : p.customParameters) o << "customParameters." << kv.first << "=" << fmt(kv.second) << "\n";
	std::vector<std::string> symbols = p.getAvailableRules();
	o << "symbols=" << join(symbols, " ") << "\n";
	for (std::string sym : symbols) {
		std::vector<Rule> rules = p.getRules(sym);
		o << "rules[" << sym << "].count=" << rules.size() << "\n";
		for (size_t i = 0; i < rules.size(); ++i) {
			std::string pre = "rules[" + sym + "][" + std::to_string(i) + "].";
			o << pre << "production=" << rules[i].production << "\n";
			o << pre << "probability=" << fmt(rules[i].probability) << "\n";
			o << pre << "initialDepth=" << rules[i].allowedDepth << "\n";
			o << pre << "parameters=" << join(rules[i].parameters, ",") << "\n";
		}
	}
	return o.str();
}

// Deterministic = every symbol has exactly one production and it is always chosen (probability 1).
bool isDeterministic(LindenmayerTreeParams& p, std::string* why = nullptr) {
	for (std::string sym : p.getAvailableRules()) {
		std::vector<Rule> rules = p.getRules(sym);
		if (rules.size() != 1) {
			if (why) *why = "symbol '" + sym + "' has " + std::to_string(rules.size()) + " productions";
			return false;
		}
		if (rules[0].probability != 1.0f) {
			if (why) *why = "symbol '" + sym + "' has probability " + fmt(rules[0].probability);
			return false;
		}
	}
	return true;
}

std::string solverGoldenText(const std::string& product) {
	if (product.size() <= kMaxStoredLength) return product;
	char buf[32];
	std::snprintf(buf, sizeof(buf), "%016llx", static_cast<unsigned long long>(fnv1a64(product)));
	return "length=" + std::to_string(product.size()) + "\nfnv1a64=" + buf + "\n";
}

std::string produce(const RuleFile& f, int depth) {
	LindenmayerTreeParams p(f.path.string());
	p.depth = depth;
	return LindenmayerTreeSolver::generateTreeProduction(p);
}

// Test-local access to the protected loader helper.
struct ProbeParams : LindenmayerTreeParams {
	ProbeParams() : LindenmayerTreeParams("", 1) {}
	using LindenmayerTreeParams::fillRuleWithVariables;
};

// Runs the solver on an in-memory rule set.
std::string solve(const std::string& axiom, int depth, const std::vector<Rule>& rules) {
	LindenmayerTreeParams p(axiom, depth);
	for (Rule r : rules) p.addRule(r);
	return LindenmayerTreeSolver::generateTreeProduction(p);
}

bool bracketsBalanced(const std::string& s) {
	int level = 0;
	for (char c : s) {
		if (c == '[') ++level;
		if (c == ']' && --level < 0) return false;
	}
	return level == 0;
}

} // namespace

// ---- loader ---------------------------------------------------------------------------------------

TEST_CASE("params: every shipped rule file is found") {
	// 21 json + 22 legacy .l at the time T02 was written; a new file must get a golden (see completeness test).
	CHECK(shippedRuleFiles().size() == 43);
}

TEST_CASE("params: every shipped rule file loads and matches its golden") {
	for (const RuleFile& f : shippedRuleFiles()) {
		CAPTURE(f.id);
		LindenmayerTreeParams p(f.path.string());
		checkGolden(kGoldenDir / "params" / (f.id + ".txt"), dumpParams(f, p));
	}
}

TEST_CASE("params: loader results are repeatable") {
	for (const RuleFile& f : shippedRuleFiles()) {
		CAPTURE(f.id);
		LindenmayerTreeParams a(f.path.string());
		LindenmayerTreeParams b(f.path.string());
		CHECK(dumpParams(f, a) == dumpParams(f, b));
	}
}

TEST_CASE("params: golden directory has no stale or missing files") {
	if (g_regenerate) return;
	std::set<std::string> expectedParams, expectedSolverPrefixes;
	for (const RuleFile& f : shippedRuleFiles()) {
		expectedParams.insert(f.id + ".txt");
		LindenmayerTreeParams p(f.path.string());
		if (isDeterministic(p)) expectedSolverPrefixes.insert(f.id + ".d");
	}
	std::set<std::string> havePrefixes;
	for (auto& e : fs::directory_iterator(kGoldenDir / "params")) {
		std::string n = e.path().filename().string();
		CHECK_MESSAGE(expectedParams.count(n) == 1, "stale params golden " << n);
		expectedParams.erase(n);
	}
	CHECK_MESSAGE(expectedParams.empty(), "missing params golden for " << (expectedParams.empty() ? "" : *expectedParams.begin()));
	for (auto& e : fs::directory_iterator(kGoldenDir / "solver")) {
		std::string n = e.path().filename().string();
		if (n.size() < 4 || n.substr(n.size() - 4) != ".txt") continue;
		std::string prefix = n.substr(0, n.rfind(".d") + 2);
		CHECK_MESSAGE(expectedSolverPrefixes.count(prefix) == 1, "stale solver golden " << n);
		havePrefixes.insert(prefix);
	}
	for (auto& p : expectedSolverPrefixes)
		CHECK_MESSAGE(havePrefixes.count(p) == 1, "missing solver goldens for " << p);
}

// ---- solver: deterministic rule files ---------------------------------------------------------------

TEST_CASE("solver: deterministic rule files match goldens at depth 0..3 and own depth") {
	size_t deterministicFiles = 0;
	for (const RuleFile& f : shippedRuleFiles()) {
		CAPTURE(f.id);
		LindenmayerTreeParams probe(f.path.string());
		if (!isDeterministic(probe)) continue;
		++deterministicFiles;

		std::set<int> depths = { 0, 1, 2, 3 };
		if (definesDepth(f)) depths.insert(probe.depth);
		for (int d : depths) {
			CAPTURE(d);
			std::string product = produce(f, d);
			checkGolden(kGoldenDir / "solver" / (f.id + ".d" + std::to_string(d) + ".txt"), solverGoldenText(product));
		}
	}
	CHECK(deterministicFiles == 34);
}

TEST_CASE("solver: deterministic output does not depend on the time-seeded static RNG") {
	// The generator is seeded from time(0) at static init; deterministic rules draw from it but the
	// outcome is fixed because probability 1 always wins (value - 1 <= 0). Repeat in-process to show it.
	for (const RuleFile& f : shippedRuleFiles()) {
		CAPTURE(f.id);
		LindenmayerTreeParams probe(f.path.string());
		if (!isDeterministic(probe)) continue;
		std::string first = produce(f, 2);
		for (int i = 0; i < 3; ++i) CHECK(produce(f, 2) == first);
	}
}

// ---- solver: stochastic rule files (properties only) ------------------------------------------------

TEST_CASE("solver: stochastic rule files keep the alphabet and bracket balance") {
	size_t stochasticFiles = 0;
	for (const RuleFile& f : shippedRuleFiles()) {
		CAPTURE(f.id);
		LindenmayerTreeParams p(f.path.string());
		if (isDeterministic(p)) continue;
		++stochasticFiles;

		// alphabet: interpreter symbols + number syntax + every character of the axiom and of all productions
		std::set<char> allowed;
		for (char c : std::string("Ffv^<>+-;`=$T[]X~(),.*/0123456789")) allowed.insert(c);
		for (char c : p.axiom) allowed.insert(c);
		for (std::string sym : p.getAvailableRules())
			for (const Rule& r : p.getRules(sym)) {
				for (char c : r.production) allowed.insert(c);
				allowed.insert(r.symbol[0]);
			}

		const int depth = definesDepth(f) ? p.depth : 3;
		std::set<std::string> distinct;
		for (int run = 0; run < 5; ++run) {
			p.depth = depth;
			std::string product = LindenmayerTreeSolver::generateTreeProduction(p);
			CHECK(product.size() > 0);
			CHECK(bracketsBalanced(product));
			bool alphabetOk = true;
			for (char c : product) alphabetOk = alphabetOk && allowed.count(c) == 1;
			CHECK_MESSAGE(alphabetOk, "unexpected character in product of " << f.id);
			distinct.insert(product);
		}
	}
	CHECK(stochasticFiles == 9);
}

TEST_CASE("solver: a stochastic rule set really varies between runs") {
	// Chance of 30 identical runs of 4 equally likely productions is negligible.
	std::set<std::string> distinct;
	for (int i = 0; i < 30; ++i)
		distinct.insert(solve("A", 1, { Rule("A", "x", 0.25f), Rule("A", "y", 0.25f), Rule("A", "z", 0.25f), Rule("A", "w", 0.25f) }));
	CHECK(distinct.size() > 1);
}

TEST_CASE("solver: probabilities below 1 can leave a symbol unrewritten") {
	// sum of probabilities 0.0 -> never chosen; value - 0 > 0 falls through to "No rules available"
	CHECK(solve("A", 3, { Rule("A", "x", 0.0f) }) == "A");
}

TEST_CASE("solver: initialDepth gates a rule by iteration index") {
	// iteration 0: rule needs allowedDepth <= 0 -> not applicable, symbol stays; iteration 1: applies
	CHECK(solve("A", 1, { Rule("A", "x", 1.0f, 1) }) == "A");
	CHECK(solve("A", 2, { Rule("A", "x", 1.0f, 1) }) == "x");
}

TEST_CASE("solver: depth 0 returns the axiom unchanged") {
	CHECK(solve("A(1)B", 0, { Rule("A", "C(l)") }) == "A(1)B");
}

TEST_CASE("solver: expression folding handles one binary operator") {
	CHECK(solve("A(2)", 1, { Rule("A", "F(l*3)", 1.0f, 0, { "l" }) }) == "F(6.000000)");
	CHECK(solve("A(2)", 1, { Rule("A", "F(l+3)", 1.0f, 0, { "l" }) }) == "F(5.000000)");
	CHECK(solve("A(2)", 1, { Rule("A", "F(l/4)", 1.0f, 0, { "l" }) }) == "F(0.500000)");
	CHECK(solve("A(2)", 1, { Rule("A", "F(l-3)", 1.0f, 0, { "l" }) }) == "F(-1.000000)");
}

TEST_CASE("params: custom parameters are substituted as std::to_string(float)") {
	ProbeParams p;
	std::map<std::string, float> vars = { { "a", 0.5f } };
	CHECK(p.fillRuleWithVariables("F(a)", vars) == "F(0.500000)");
}

// ---- known bugs: these assert the CURRENT, WRONG behavior (see tests/golden/KNOWN_BUGS.md) -------------

TEST_CASE("KNOWN BUG: loader substitution skips the second of two adjacent identical parameters") {
	// Each match also consumes its closing delimiter, so in "(a1,a1)" the ',' is eaten by the first match
	// and the second "a1" has no leading delimiter left. Correct output: "F(35.000000,35.000000)". Owner: T03.
	ProbeParams p;
	std::map<std::string, float> vars = { { "a1", 35.0f } };
	CHECK(p.fillRuleWithVariables("F(a1,a1)", vars) == "F(35.000000,a1)");
}

TEST_CASE("KNOWN BUG: loader substitution needs a delimiter before the parameter name") {
	// A parameter at the very start of a production is not replaced. Correct: "35.000000F". Owner: T03.
	ProbeParams p;
	std::map<std::string, float> vars = { { "a1", 35.0f } };
	CHECK(p.fillRuleWithVariables("a1F", vars) == "a1F");
}

TEST_CASE("params: substitution keeps the text after the last match") {
	// Characterizes res.suffix() read after the failed final regex_search (see KNOWN_BUGS.md, "suspected, not confirmed").
	ProbeParams p;
	std::map<std::string, float> vars = { { "a1", 35.0f } };
	CHECK(p.fillRuleWithVariables("[v(a1)B]C", vars) == "[v(35.000000)B]C");
}

TEST_CASE("KNOWN BUG: solver substitution skips the second of two adjacent identical parameters") {
	// Same defect in LindenmayerTreeSolver::fillRuleWithParameters. Correct: "F(2,2)". Owner: T04.
	CHECK(solve("A(2)", 1, { Rule("A", "F(x,x)", 1.0f, 0, { "x" }) }) == "F(2,x)");
}

TEST_CASE("KNOWN BUG: expression with a leading minus is mis-evaluated") {
	// "(-5*2" is split at the first operator char '-': left "" -> 0, right "5*2" -> atof 5 => 0 - 5.
	// Correct: F(-10.000000). Owner: T04.
	CHECK(solve("A", 1, { Rule("A", "F(-5*2)") }) == "F(-5.000000)");
}

TEST_CASE("KNOWN BUG: a negative parameter value in an expression is mis-evaluated") {
	// l = -2 gives "F(-2*3)" which hits the leading-minus defect. Correct: F(-6.000000). Owner: T04.
	CHECK(solve("A(-2)", 1, { Rule("A", "F(l*3)", 1.0f, 0, { "l" }) }) == "F(-2.000000)");
}

TEST_CASE("KNOWN BUG: chained operators are not evaluated") {
	// Split at the first operator: left 2, right "3*4" -> atof 3 => 2*3. Correct: F(24.000000). Owner: T04.
	CHECK(solve("A", 1, { Rule("A", "F(2*3*4)") }) == "F(6.000000)");
	// No precedence: 1+2*3 is read as 1 + atof("2*3") = 3. Correct: F(7.000000).
	CHECK(solve("A", 1, { Rule("A", "F(1+2*3)") }) == "F(3.000000)");
}

TEST_CASE("KNOWN BUG: a rule that rewrites everything to the empty string is ignored") {
	// `if (newProduct.length() > 0) product = newProduct;` keeps the old product. Correct: "". Owner: T04.
	CHECK(solve("A", 1, { Rule("A", "") }) == "A");
}

TEST_CASE("KNOWN BUG: rule symbols that are regex metacharacters make the solver throw") {
	// generateSymbolRegex() concatenates symbols unescaped: "(+|A)" is an invalid ECMAScript regex.
	// '+' '^' '*' etc. are valid turtle symbols. Correct: they can be rewritten. Owner: T04.
	CHECK_THROWS_AS(solve("A", 1, { Rule("+", "A"), Rule("A", "B") }), std::regex_error);
}

TEST_CASE("KNOWN BUG: a rule with parameters applied to a symbol without arguments leaves the names in place") {
	// Arity mismatch: no value list, so `l` is never substituted and the expression is not folded.
	// Correct: a typed error naming rule and symbol (docs/PLAN.md Review Focus 4). Owner: T04.
	CHECK(solve("A", 1, { Rule("A", "F(l*2)", 1.0f, 0, { "l" }) }) == "F(l*2)");
}

TEST_CASE("KNOWN BUG: custom parameters are not substituted into the axiom") {
	// legacy/parametricTree2.l has axiom "B(1)F(startLength)+(45)A" with `define startLength 1.2`.
	// Correct: "B(1)F(1.200000)+(45)A". Owner: T03.
	LindenmayerTreeParams p((kRulesDir / "legacy" / "parametricTree2.l").string());
	CHECK(p.axiom == "B(1)F(startLength)+(45)A");
}

TEST_CASE("KNOWN BUG: bare-number header lines of old .l files are read as axioms") {
	// testX.l and advancedTree.l start with "4 / 0.5 / 0.25 / 7" (depth, length, radius, angle), but the loader
	// treats every one-token line as the axiom (last wins) and leaves depth/length/radius/angle at defaults.
	// Correct: depth, initialLength, initialRadius, angle taken from the header. Owner: T03.
	LindenmayerTreeParams p((kRulesDir / "legacy" / "advancedTree.l").string());
	CHECK(p.axiom == "F");
	CHECK(p.initialLength == 1.0f);
	CHECK(p.initialRadius == 1.0f);
	CHECK(p.angle == 15.0f); // default is stored in degrees-as-radians: 15.0, not radians(15)
}

TEST_CASE("KNOWN BUG: '.l' files ignore the tropism define and the default angle is not converted") {
	// `define tropism ...` is a TODO in readLFile; the 15.0 default angle is not run through radians().
	// Correct: defaults radians(15) = 0.261799. Owner: T03.
	LindenmayerTreeParams p((kRulesDir / "legacy" / "normalTest.l").string());
	CHECK(p.tropism == glm::vec3(0, -1, 0));
	LindenmayerTreeParams q("A", 1);
	CHECK(q.angle == doctest::Approx(0.261799f)); // the string/axiom constructor converts...
	LindenmayerTreeParams r((kRulesDir / "legacy" / "testX.l").string());
	CHECK(r.angle == 15.0f); // ...the file loader's untouched default does not
}

// ---- main -------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
	for (int i = 1; i < argc; ++i)
		if (std::string(argv[i]) == "--regenerate") g_regenerate = true;

	doctest::Context ctx;
	ctx.applyCommandLine(argc, argv);
	ctx.setOption("no-breaks", true);
	return ctx.run();
}
