// Old-versus-new: treegen::buildGeometry against the frozen copy of the legacy ring/index math
// (tests/treegen/legacy_reference/LegacyGeometryReference.h, deleted in T07). Both are fed the same skeleton, so
// only the geometry code is compared (the turtle has its own equivalence tests).
#include <doctest/doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <string>
#include <vector>

#include <treegen/Geometry.h>
#include <treegen/Module.h>
#include <treegen/Rewriter.h>
#include <treegen/Skeleton.h>
#include <treegen/TreeParams.h>
#include <treegen/Turtle.h>

#include "legacy_reference/LegacyGeometryReference.h"

namespace {

const float kEpsilon = 1e-4f;

std::vector<legacy_reference::GeometrySegment> toReference(const treegen::Skeleton& skeleton) {
	std::vector<legacy_reference::GeometrySegment> out;
	for (const treegen::SkeletonSegment& s : skeleton.segments) {
		legacy_reference::GeometrySegment r;
		r.parent = s.parent;
		r.model = s.model;
		r.radius = s.radius;
		r.roll = s.roll;
		out.push_back(r);
	}
	return out;
}

struct Deviation {
	size_t vertices = 0, indices = 0;
	float position = 0, normal = 0, uv = 0;
	bool sameIndices = true;
};

Deviation compareToLegacy(const treegen::Skeleton& skeleton, const treegen::TreeParams& params, int ringDensity, unsigned threads) {
	treegen::GeometryOptions options;
	options.ringDensity = ringDensity;
	options.threads = threads;
	treegen::TreeGeometry now = treegen::buildGeometry(skeleton, params, options);
	legacy_reference::GeometryResult then = legacy_reference::generateTrunk(toReference(skeleton), ringDensity);

	Deviation d;
	d.vertices = now.trunk.vertices.size();
	d.indices = now.trunk.indices.size();
	REQUIRE(then.vertices.size() == now.trunk.vertices.size());
	REQUIRE(then.indices.size() == now.trunk.indices.size());

	d.sameIndices = std::equal(then.indices.begin(), then.indices.end(), now.trunk.indices.begin(),
		[](int a, uint32_t b) { return a >= 0 && uint32_t(a) == b; });
	for (size_t i = 0; i < then.vertices.size(); ++i) {
		const legacy_reference::GeometryVertex& a = then.vertices[i];
		const treegen::MeshVertex& b = now.trunk.vertices[i];
		auto worst = [](const float* x, const float* y, int n) {
			float w = 0;
			for (int k = 0; k < n; ++k) {
				float diff = std::fabs(x[k] - y[k]);
				if (!(diff == diff))
					diff = 1e9f;   // a NaN must not pass as "small"
				w = std::max(w, diff);
			}
			return w;
		};
		d.position = std::max(d.position, worst(&a.position.x, &b.position.x, 3));
		d.normal = std::max(d.normal, worst(&a.normal.x, &b.normal.x, 3));
		d.uv = std::max(d.uv, worst(&a.texCoords.x, &b.uv.x, 2));
	}
	return d;
}

void expectParity(const treegen::Skeleton& skeleton, const treegen::TreeParams& params, const std::string& label) {
	INFO(label);
	for (int ringDensity : { 9, 5 })
		for (unsigned threads : { 1u, 3u }) {
			INFO("ringDensity " << ringDensity << " threads " << threads);
			Deviation d = compareToLegacy(skeleton, params, ringDensity, threads);
			CHECK(d.vertices > 0);
			CHECK(d.sameIndices);
			CHECK(d.position <= kEpsilon);
			CHECK(d.normal <= kEpsilon);
			CHECK(d.uv <= kEpsilon);
		}
}

void parityOnRuleFile(const std::string& relative, int depth) {
	const std::filesystem::path file = std::filesystem::path(ESTREE_REPO_ROOT) / "Resources" / "LindenmayerRules" / relative;
	treegen::TreeParams params = treegen::loadTreeParams(file);
	params.depth = std::min(params.depth, depth);

	treegen::RewriteOptions rewriteOptions;
	rewriteOptions.seed = 7;
	treegen::ModuleString modules = treegen::rewrite(params, rewriteOptions);
	treegen::Skeleton skeleton = treegen::interpret(params, modules);
	REQUIRE(skeleton.segments.size() > 20);   // the comparison is not vacuous
	expectParity(skeleton, params, relative + " depth " + std::to_string(params.depth) + " (" + std::to_string(skeleton.segments.size()) + " segments)");
}

}

TEST_CASE("Geometry parity: parametricTernaryTreeE.json (tropism, rolls, scaling)") {
	parityOnRuleFile("parametricTernaryTreeE.json", 4);
}

TEST_CASE("Geometry parity: parametricSympodialTreeA.json") {
	parityOnRuleFile("parametricSympodialTreeA.json", 7);
}

TEST_CASE("Geometry parity: parametricMonopodialTreeA.json") {
	parityOnRuleFile("parametricMonopodialTreeA.json", 7);
}

TEST_CASE("Geometry parity: symetricTree.json") {
	parityOnRuleFile("symetricTree.json", 4);
}

TEST_CASE("Geometry parity: legacy/advancedTree.l (old header format)") {
	parityOnRuleFile("legacy/advancedTree.l", 3);
}

TEST_CASE("Geometry parity: rolls and a changing radius along a hand-written production") {
	treegen::TreeParams params;
	params.initialLength = 2.0f;
	params.initialRadius = 0.8f;
	treegen::Skeleton skeleton = treegen::interpret(params, treegen::parseModules(
		"F(2)+(35)F(1)=(0.7)-(80)^(30)F(1.5)[+(120)<(40)`(0.8)F(1)F(1)][-(60)v(25)=(0.5)F(2)]$F(1)+(200)F(1)"));
	REQUIRE(skeleton.segments.size() > 8);
	expectParity(skeleton, params, "hand-written");
}

TEST_CASE("Geometry parity: the reference reproduces the F(1) counts used by the geometry tests") {
	treegen::TreeParams params;
	treegen::Skeleton skeleton = treegen::interpret(params, treegen::parseModules("F(1)"));
	legacy_reference::GeometryResult then = legacy_reference::generateTrunk(toReference(skeleton), 9);
	CHECK(then.vertices.size() == 18);
	CHECK(then.indices.size() == 48);
}
