// Old-versus-new: treegen::interpret against the frozen copy of the legacy turtle math
// (tests/treegen/legacy_reference, deleted in T07) on productions of the T02 solver goldens.
//
// Two comparisons per production:
//   - "reference + normalisation": the legacy math with the one documented change (the accumulated rotation is
//     renormalised after each F). Must match treegen::interpret to 1e-4 for EVERY production, with or without T.
//   - "verbatim reference": no change at all. Must match to 1e-4 for productions without T; with T the legacy
//     accumulated rotation shrinks (norm < 1), which treegen does not reproduce, so only a sanity bound is checked.
// Both start the legacy turtle from the identity quaternion (see D-010 and the as-compiled tests at the end).
#include <doctest/doctest.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <treegen/Error.h>
#include <treegen/Module.h>
#include <treegen/TreeParams.h>
#include <treegen/Turtle.h>

#include "legacy_reference/LegacyTurtleReference.h"

namespace fs = std::filesystem;

namespace {

const fs::path kRepoRoot = ESTREE_REPO_ROOT;
const fs::path kGoldenDir = kRepoRoot / "tests" / "golden" / "solver";
const fs::path kRulesDir = kRepoRoot / "Resources" / "LindenmayerRules";

const float kEpsilon = 1e-4f;

struct Deviation {
	float model = 0;
	float radius = 0;
	float roll = 0;
	size_t segments = 0;
	bool structureEqual = true;

	float worst() const { return std::max(model, std::max(radius, roll)); }
};

std::string readFile(const fs::path& file) {
	std::ifstream in(file, std::ios::binary);
	REQUIRE_MESSAGE(in.good(), "cannot open " << file.string());
	return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

// "legacy__parametricTree.l" -> Resources/LindenmayerRules/legacy/parametricTree.l
treegen::TreeParams loadRules(const std::string& id) {
	std::string relative = id;
	for (size_t at = relative.find("__"); at != std::string::npos; at = relative.find("__"))
		relative.replace(at, 2, "/");
	return treegen::loadTreeParams(kRulesDir / relative);
}

legacy_reference::Params referenceParams(const treegen::TreeParams& p) {
	legacy_reference::Params r;
	r.initialLength = p.initialLength;
	r.initialRadius = p.initialRadius;
	r.angle = p.angle;
	r.tropismBendingFactor = p.tropismBendingFactor;
	r.tropism = p.tropism;
	return r;
}

treegen::Skeleton toSkeleton(const std::vector<legacy_reference::Segment>& segments) {
	treegen::Skeleton s;
	for (const legacy_reference::Segment& seg : segments)
		s.segments.push_back({ seg.parent, seg.model, seg.radius, seg.roll, seg.childCount });
	return s;
}

Deviation compare(const treegen::Skeleton& now, const std::vector<legacy_reference::Segment>& then) {
	Deviation d;
	d.segments = now.segments.size();
	if (now.segments.size() != then.size()) {
		d.structureEqual = false;
		return d;
	}
	for (size_t i = 0; i < then.size(); ++i) {
		const treegen::SkeletonSegment& a = now.segments[i];
		const legacy_reference::Segment& b = then[i];
		if (a.parent != b.parent || a.childCount != b.childCount)
			d.structureEqual = false;
		for (int c = 0; c < 4; ++c)
			for (int r = 0; r < 4; ++r) {
				float diff = std::fabs(a.model[c][r] - b.model[c][r]);
				if (!(diff == diff))   // NaN must not pass as "small"
					diff = 1e9f;
				d.model = std::max(d.model, diff);
			}
		d.radius = std::max(d.radius, std::fabs(a.radius - b.radius));
		d.roll = std::max(d.roll, std::fabs(a.roll - b.roll));
	}
	return d;
}

struct Both {
	Deviation normalized;
	Deviation verbatim;
	bool hasTropism = false;
};

Both compareBoth(const treegen::TreeParams& params, const std::string& production) {
	treegen::Skeleton now = treegen::interpret(params, treegen::parseModules(production));
	Both both;
	both.normalized = compare(now, legacy_reference::interpret(referenceParams(params), production, legacy_reference::QuatInit::Identity, true));
	both.verbatim = compare(now, legacy_reference::interpret(referenceParams(params), production, legacy_reference::QuatInit::Identity, false));
	both.hasTropism = production.find('T') != std::string::npos;
	return both;
}

void expectEquivalent(const std::string& id, int depth, const std::string& label) {
	const std::string production = readFile(kGoldenDir / (id + ".d" + std::to_string(depth) + ".txt"));
	Both both = compareBoth(loadRules(id), production);
	std::cout << "[equivalence] " << label << ": " << both.normalized.segments << " segments, max deviation vs reference+normalisation "
	          << both.normalized.worst() << ", vs verbatim legacy " << both.verbatim.worst() << (both.hasTropism ? " (uses T)" : "") << std::endl;
	CHECK(both.normalized.segments > 1);
	CHECK_MESSAGE(both.normalized.structureEqual, label << ": parent/childCount differ");
	CHECK_MESSAGE(both.normalized.model <= kEpsilon, label << ": model matrices deviate by " << both.normalized.model);
	CHECK_MESSAGE(both.normalized.radius <= kEpsilon, label << ": radius deviates by " << both.normalized.radius);
	CHECK_MESSAGE(both.normalized.roll <= kEpsilon, label << ": roll deviates by " << both.normalized.roll);
	CHECK_MESSAGE(both.verbatim.structureEqual, label << ": parent/childCount differ from the verbatim legacy");
	if (both.hasTropism)
		CHECK_MESSAGE(both.verbatim.model < 0.25f, label << ": deviation from the verbatim legacy " << both.verbatim.model);
	else
		CHECK_MESSAGE(both.verbatim.worst() <= kEpsilon, label << ": deviation from the verbatim legacy " << both.verbatim.worst());
}

}

TEST_CASE("Turtle equivalence: monopodial tree A with dollar and roll, depth 3") {
	expectEquivalent("parametricMonopodialTreeA.json", 3, "parametricMonopodialTreeA.json d3");
}

TEST_CASE("Turtle equivalence: sympodial tree B, depth 3") {
	expectEquivalent("parametricSympodialTreeB.json", 3, "parametricSympodialTreeB.json d3");
}

TEST_CASE("Turtle equivalence: legacy symmetric tree without parameters, depth 4") {
	expectEquivalent("legacy__symetricTree.l", 4, "legacy/symetricTree.l d4");
}

TEST_CASE("Turtle equivalence: legacy test tree with dollar, depth 3") {
	expectEquivalent("legacy__test.l", 3, "legacy/test.l d3");
}

TEST_CASE("Turtle equivalence: advanced tree, depth 3") {
	expectEquivalent("advancedTree.json", 3, "advancedTree.json d3");
}

TEST_CASE("Turtle equivalence: legacy parametric tree, depth 3") {
	expectEquivalent("legacy__parametricTree.l", 3, "legacy/parametricTree.l d3");
}

TEST_CASE("Turtle equivalence: ternary tree B uses tropism, depth 3") {
	expectEquivalent("parametricTernaryTreeB.json", 3, "parametricTernaryTreeB.json d3");
}

TEST_CASE("Turtle equivalence: ternary tree C uses tropism, depth 3") {
	expectEquivalent("parametricTernaryTreeC.json", 3, "parametricTernaryTreeC.json d3");
}

TEST_CASE("Turtle equivalence: every solver golden up to depth 4") {
	// The legacy solver of parametricTree2.l emitted text that is not a module string (a lost '(' in "^+18.95)"):
	// the legacy turtle skipped the garbage character by character. The T04 rewriter does not produce such text.
	const std::vector<std::string> notModuleStrings = {
		"legacy__parametricTree2.l.d1.txt", "legacy__parametricTree2.l.d2.txt", "legacy__parametricTree2.l.d3.txt" };

	size_t compared = 0, tropism = 0;
	float worstNormalized = 0, worstVerbatimNoT = 0, worstVerbatimT = 0;
	std::string worstName;
	std::vector<std::string> skipped;
	for (const fs::directory_entry& entry : fs::directory_iterator(kGoldenDir)) {
		std::string name = entry.path().filename().string();   // <id>.dN.txt
		if (name.size() < 7 || name.substr(name.size() - 4) != ".txt")
			continue;
		size_t dot = name.rfind(".d", name.size() - 5);
		if (dot == std::string::npos)
			continue;
		int depth = std::atoi(name.substr(dot + 2, name.size() - dot - 6).c_str());
		if (depth > 4 || fs::file_size(entry.path()) > 100000)
			continue;
		const std::string production = readFile(entry.path());
		if (production.rfind("length=", 0) == 0)
			continue;
		if (std::find(notModuleStrings.begin(), notModuleStrings.end(), name) != notModuleStrings.end()) {
			REQUIRE_THROWS_AS(treegen::parseModules(production), treegen::RewriteError);
			skipped.push_back(name);
			continue;
		}

		Both both = compareBoth(loadRules(name.substr(0, dot)), production);
		CHECK_MESSAGE(both.normalized.structureEqual, name << ": parent/childCount differ");
		CHECK_MESSAGE(both.normalized.model <= kEpsilon, name << ": model deviates by " << both.normalized.model);
		CHECK_MESSAGE(both.normalized.radius <= kEpsilon, name << ": radius deviates by " << both.normalized.radius);
		CHECK_MESSAGE(both.normalized.roll <= kEpsilon, name << ": roll deviates by " << both.normalized.roll);
		if (both.hasTropism) {
			++tropism;
			CHECK_MESSAGE(both.verbatim.model < 0.25f, name << ": deviation from the verbatim legacy " << both.verbatim.model);
			worstVerbatimT = std::max(worstVerbatimT, both.verbatim.model);
		} else {
			CHECK_MESSAGE(both.verbatim.worst() <= kEpsilon, name << ": deviation from the verbatim legacy " << both.verbatim.worst());
			worstVerbatimNoT = std::max(worstVerbatimNoT, both.verbatim.worst());
		}
		if (both.normalized.model >= worstNormalized) {
			worstNormalized = both.normalized.model;
			worstName = name;
		}
		++compared;
	}
	std::cout << "[equivalence] sweep: " << compared << " goldens (" << tropism << " with T), skipped " << skipped.size()
	          << "; worst deviation vs reference+normalisation " << worstNormalized << " (" << worstName
	          << "), vs verbatim legacy without T " << worstVerbatimNoT << ", with T " << worstVerbatimT << std::endl;
	CHECK(compared >= 100);
	CHECK(tropism >= 10);
	// T04 deleted these three goldens (its rewriter reports the typo instead of copying it), so none may be present;
	// if they come back, the loop above has already asserted that they are not module strings.
	CHECK(skipped.size() <= notModuleStrings.size());
}

// ---- what the shipped binary actually did (documented in D-010) -----------------------------------------------

TEST_CASE("Turtle equivalence: as compiled, the legacy zero quaternion start loses rotations before the first F") {
	legacy_reference::Params p;
	p.angle = 1.5707964f;
	auto legacy = legacy_reference::interpret(p, "^(90)F(1)", legacy_reference::QuatInit::AsCompiledLegacy);
	auto identity = legacy_reference::interpret(p, "^(90)F(1)", legacy_reference::QuatInit::Identity);
	REQUIRE(legacy.size() == 2);
	CHECK(std::fabs(legacy[1].model[3][1] - 1.0f) < kEpsilon);   // still straight up: the pitch was multiplied into 0
	CHECK(std::fabs(legacy[1].model[3][2]) < kEpsilon);
	CHECK(std::fabs(identity[1].model[3][2] - 1.0f) < kEpsilon);   // what the table in overview.md says
}

TEST_CASE("Turtle equivalence: as compiled, the legacy tropism T is a no-op") {
	const std::string id = "parametricTernaryTreeB.json";
	treegen::TreeParams params = loadRules(id);
	const std::string production = readFile(kGoldenDir / (id + ".d3.txt"));
	std::string withoutT;
	for (char c : production)
		if (c != 'T')
			withoutT += c;
	REQUIRE(withoutT.size() < production.size());

	auto asCompiled = legacy_reference::interpret(referenceParams(params), production, legacy_reference::QuatInit::AsCompiledLegacy);
	auto asCompiledNoT = legacy_reference::interpret(referenceParams(params), withoutT, legacy_reference::QuatInit::AsCompiledLegacy);
	Deviation sameTree = compare(toSkeleton(asCompiledNoT), asCompiled);
	std::cout << "[equivalence] as compiled, tree with T vs the same string without T: max |model| " << sameTree.model << std::endl;
	CHECK(sameTree.structureEqual);
	CHECK(sameTree.model <= kEpsilon);

	// treegen implements the working tropism, so it differs from the as-compiled tree
	treegen::Skeleton now = treegen::interpret(params, treegen::parseModules(production));
	Deviation toNow = compare(now, asCompiled);
	std::cout << "[equivalence] treegen vs as-compiled legacy on " << id << " d3: max |model| " << toNow.model << std::endl;
	CHECK(toNow.model > 0.05f);
}

TEST_CASE("Turtle equivalence: productions without T or pre-first-F rotations are identical under both quaternion starts") {
	for (const char* id : { "parametricMonopodialTreeA.json", "parametricSympodialTreeB.json" }) {
		treegen::TreeParams params = loadRules(id);
		const std::string production = readFile(kGoldenDir / (std::string(id) + ".d3.txt"));
		auto asCompiled = legacy_reference::interpret(referenceParams(params), production, legacy_reference::QuatInit::AsCompiledLegacy);
		Deviation d = compare(treegen::interpret(params, treegen::parseModules(production)), asCompiled);
		CHECK_MESSAGE(d.worst() <= kEpsilon, id << ": treegen differs from the as-compiled legacy by " << d.worst());
	}
}
