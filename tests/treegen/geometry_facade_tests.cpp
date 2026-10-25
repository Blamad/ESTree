// treegen::generateTree: the rewrite -> interpret -> geometry facade.
#include <doctest/doctest.h>

#include <cmath>
#include <cstring>
#include <filesystem>
#include <string>

#include <treegen/TreeGenerator.h>

namespace {

treegen::TreeParams loadRules(const std::string& relative) {
	return treegen::loadTreeParams(std::filesystem::path(ESTREE_REPO_ROOT) / "Resources" / "LindenmayerRules" / relative);
}

bool allFinite(const treegen::TreeGeometry& g) {
	for (const treegen::MeshVertex& v : g.trunk.vertices) {
		const float* f = &v.position.x;
		for (int i = 0; i < 8; ++i)
			if (!std::isfinite(f[i]))
				return false;
	}
	for (const glm::mat4& m : g.leafTransforms)
		for (int c = 0; c < 4; ++c)
			for (int r = 0; r < 4; ++r)
				if (!std::isfinite(m[c][r]))
					return false;
	return true;
}

bool sameTrunk(const treegen::MeshData& a, const treegen::MeshData& b) {
	return a.indices == b.indices && a.vertices.size() == b.vertices.size()
		&& (a.vertices.empty() || std::memcmp(a.vertices.data(), b.vertices.data(), a.vertices.size() * sizeof(treegen::MeshVertex)) == 0);
}

bool sameSkeleton(const treegen::Skeleton& a, const treegen::Skeleton& b) {
	if (a.segments.size() != b.segments.size())
		return false;
	for (size_t i = 0; i < a.segments.size(); ++i)
		if (a.segments[i].parent != b.segments[i].parent || a.segments[i].model != b.segments[i].model ||
			a.segments[i].radius != b.segments[i].radius || a.segments[i].roll != b.segments[i].roll)
			return false;
	return true;
}

}

TEST_CASE("generateTree: parametricTernaryTreeE.json end to end") {
	treegen::TreeParams params = loadRules("parametricTernaryTreeE.json");
	REQUIRE(params.depth >= 5);
	params.depth = 5;
	params.leafGrowthProb = 1.0f;

	treegen::GenerateOptions options;
	options.seed = 3;
	treegen::TreeModel model = treegen::generateTree(params, options);

	CHECK(!model.modules.empty());
	REQUIRE(model.skeleton.segments.size() > 100);

	const size_t segments = model.skeleton.segments.size() - 1;
	const treegen::TreeGeometry& g = model.geometry;
	CHECK(g.stats.segments == segments);
	CHECK(g.stats.emittedSegments == segments);   // nothing degenerate in a shipped file
	CHECK(g.trunk.vertices.size() == segments * 18);
	CHECK(g.trunk.indices.size() == segments * 48);
	CHECK(allFinite(g));

	size_t tips = 0;
	for (const treegen::SkeletonSegment& s : model.skeleton.segments)
		if (treegen::isTip(s))
			++tips;
	CHECK(g.leafTransforms.size() == tips);
	for (uint32_t index : g.trunk.indices)
		CHECK(index < g.trunk.vertices.size());
}

TEST_CASE("generateTree: the facade is rewrite, interpret and buildGeometry with one seed") {
	treegen::TreeParams params = loadRules("randomTree.json");
	params.depth = 4;
	params.leafGrowthProb = 0.5f;

	treegen::GenerateOptions options;
	options.seed = 21;
	options.ringDensity = 7;
	treegen::TreeModel model = treegen::generateTree(params, options);

	treegen::RewriteOptions rewriteOptions;
	rewriteOptions.seed = 21;
	treegen::ModuleString modules = treegen::rewrite(params, rewriteOptions);
	CHECK(treegen::toString(model.modules) == treegen::toString(modules));

	treegen::InterpretOptions interpretOptions;
	interpretOptions.seed = 21;
	treegen::Skeleton skeleton = treegen::interpret(params, modules, interpretOptions);
	CHECK(sameSkeleton(model.skeleton, skeleton));

	treegen::GeometryOptions geometryOptions;
	geometryOptions.ringDensity = 7;
	geometryOptions.seed = 21;
	treegen::TreeGeometry geometry = treegen::buildGeometry(skeleton, params, geometryOptions);
	CHECK(sameTrunk(model.geometry.trunk, geometry.trunk));
	CHECK(model.geometry.leafTransforms == geometry.leafTransforms);
	CHECK(model.geometry.trunk.vertices.size() == (skeleton.segments.size() - 1) * 14);
}

TEST_CASE("generateTree: determinism, seed 0 means params.seed, and the seed matters") {
	treegen::TreeParams params = loadRules("randomTree.json");
	params.depth = 4;
	params.leafGrowthProb = 0.5f;

	treegen::GenerateOptions options;
	options.seed = 5;
	treegen::TreeModel a = treegen::generateTree(params, options);
	treegen::TreeModel b = treegen::generateTree(params, options);
	CHECK(treegen::toString(a.modules) == treegen::toString(b.modules));
	CHECK(sameSkeleton(a.skeleton, b.skeleton));
	CHECK(sameTrunk(a.geometry.trunk, b.geometry.trunk));
	CHECK(a.geometry.leafTransforms == b.geometry.leafTransforms);

	// params.seed is used when the option is 0
	treegen::TreeParams seeded = params;
	seeded.seed = 5;
	treegen::GenerateOptions fromParams;
	treegen::TreeModel c = treegen::generateTree(seeded, fromParams);
	CHECK(treegen::toString(a.modules) == treegen::toString(c.modules));
	CHECK(a.geometry.leafTransforms == c.geometry.leafTransforms);

	// an explicit seed wins over params.seed
	seeded.seed = 999;
	treegen::TreeModel d = treegen::generateTree(seeded, options);
	CHECK(treegen::toString(a.modules) == treegen::toString(d.modules));

	// another seed gives another tree (rule choice, and the leaf set)
	options.seed = 6;
	treegen::TreeModel e = treegen::generateTree(params, options);
	CHECK(treegen::toString(a.modules) != treegen::toString(e.modules));
}

TEST_CASE("generateTree: the thread count is not observable in the result") {
	treegen::TreeParams params = loadRules("parametricTernaryTreeE.json");
	params.depth = 7;
	params.leavesAngleDiversity = 0.5f;

	treegen::GenerateOptions options;
	options.seed = 2;
	options.threads = 1;
	treegen::TreeModel single = treegen::generateTree(params, options);
	REQUIRE(single.geometry.trunk.vertices.size() > 50000);
	for (unsigned threads : { 0u, 2u, 5u }) {
		options.threads = threads;
		treegen::TreeModel other = treegen::generateTree(params, options);
		INFO("threads " << threads);
		CHECK(sameTrunk(single.geometry.trunk, other.geometry.trunk));
		CHECK(single.geometry.leafTransforms == other.geometry.leafTransforms);
	}
}

TEST_CASE("generateTree: leaf settings change only the leaves") {
	treegen::TreeParams params = loadRules("randomTree.json");
	params.depth = 4;
	treegen::GenerateOptions options;
	options.seed = 8;

	params.leafGrowthProb = 1.0f;
	treegen::TreeModel all = treegen::generateTree(params, options);
	params.leafGrowthProb = 0.25f;
	treegen::TreeModel some = treegen::generateTree(params, options);

	CHECK(treegen::toString(all.modules) == treegen::toString(some.modules));
	CHECK(sameSkeleton(all.skeleton, some.skeleton));
	CHECK(sameTrunk(all.geometry.trunk, some.geometry.trunk));
	CHECK(some.geometry.leafTransforms.size() < all.geometry.leafTransforms.size());
	CHECK(!some.geometry.leafTransforms.empty());
}

TEST_CASE("generateTree: errors of the stages reach the caller") {
	treegen::TreeParams broken = treegen::parseTreeParamsJson(
		R"json({ "axiom": "A", "parameters": { "depth": 1 }, "rules": [ { "symbol": "A", "production": "F(1)[+(30)A)" } ] })json", "broken.json");
	CHECK_THROWS_AS(treegen::generateTree(broken), treegen::RewriteError);

	treegen::TreeParams unbalanced;
	unbalanced.axiom = "F(1)]";
	CHECK_THROWS_AS(treegen::generateTree(unbalanced), treegen::InterpretError);

	treegen::TreeParams fine;
	fine.axiom = "F(1)";
	treegen::GenerateOptions options;
	options.ringDensity = 2;
	CHECK_THROWS_AS(treegen::generateTree(fine, options), treegen::Error);
}

TEST_CASE("generateTree: a tree without rules or segments is just a root") {
	treegen::TreeParams params;
	params.axiom = "A";
	treegen::TreeModel model = treegen::generateTree(params);
	CHECK(model.skeleton.segments.size() == 1);
	CHECK(model.geometry.trunk.vertices.empty());
	CHECK(model.geometry.trunk.indices.empty());
	CHECK(model.geometry.leafTransforms.size() == 1);   // the root is a tip, as in the legacy loop
}
