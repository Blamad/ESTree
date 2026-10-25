// The params goldens of tests/golden/params were produced by the legacy loader (T02). treegen's loader has to
// reproduce them byte for byte, on every platform; the few intentional differences were regenerated with a
// DECISIONS.md entry (see tests/golden/KNOWN_BUGS.md). The legacy pipeline that produced them
// was deleted in T07.
#include <doctest/doctest.h>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <treegen/TreeParams.h>

namespace fs = std::filesystem;

namespace {

const fs::path kRepoRoot = fs::path(ESTREE_REPO_ROOT);
const fs::path kRulesDir = kRepoRoot / "Resources" / "LindenmayerRules";
const fs::path kGoldenDir = kRepoRoot / "tests" / "golden" / "params";

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

std::string readFile(const fs::path& p, bool* ok = nullptr) {
	std::ifstream in(p, std::ios::binary);
	if (ok)
		*ok = static_cast<bool>(in);
	std::stringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

std::string fmt(double v) {
	char buf[64];
	std::snprintf(buf, sizeof(buf), "%.9g", v);
	return buf;
}

// Same rule as in the legacy test: a .l file sets the depth with `define depth` or with a bare number as its
// first line; otherwise the golden prints UNSET.
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

std::string join(const std::vector<std::string>& v, const char* sep) {
	std::string s;
	for (size_t i = 0; i < v.size(); ++i)
		s += (i ? sep : "") + v[i];
	return s;
}

// The format of the dump the legacy characterization test wrote (T02).
std::string dumpParams(const RuleFile& f, const treegen::TreeParams& p) {
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
	for (auto& kv : p.customParameters)
		o << "customParameters." << kv.first << "=" << fmt(kv.second) << "\n";
	std::vector<std::string> symbols;
	for (auto& kv : p.rules)
		symbols.push_back(std::string(1, kv.first));
	o << "symbols=" << join(symbols, " ") << "\n";
	for (auto& kv : p.rules) {
		const std::string sym(1, kv.first);
		o << "rules[" << sym << "].count=" << kv.second.size() << "\n";
		for (size_t i = 0; i < kv.second.size(); ++i) {
			const treegen::Rule& rule = kv.second[i];
			std::string pre = "rules[" + sym + "][" + std::to_string(i) + "].";
			o << pre << "production=" << rule.production << "\n";
			o << pre << "probability=" << fmt(rule.probability) << "\n";
			o << pre << "initialDepth=" << rule.initialDepth << "\n";
			o << pre << "parameters=" << join(rule.paramNames, ",") << "\n";
		}
	}
	return o.str();
}

}

TEST_CASE("Golden: treegen's loader reproduces every params golden of the shipped rule files") {
	const std::vector<RuleFile> files = shippedRuleFiles();
	CHECK(files.size() == 43);
	for (const RuleFile& f : files) {
		CAPTURE(f.id);
		bool ok = false;
		const std::string expected = readFile(kGoldenDir / (f.id + ".txt"), &ok);
		REQUIRE_MESSAGE(ok, "missing golden for " << f.id);
		const std::string actual = dumpParams(f, treegen::loadTreeParams(f.path));
		CHECK_MESSAGE(expected == actual, "golden mismatch for " << f.id);
	}
}

TEST_CASE("Golden: loading is repeatable") {
	for (const RuleFile& f : shippedRuleFiles()) {
		CAPTURE(f.id);
		CHECK(dumpParams(f, treegen::loadTreeParams(f.path)) == dumpParams(f, treegen::loadTreeParams(f.path)));
	}
}
