// Header self-containment: <treegen/TreeGenerator.h> must be enough for a consumer. This translation unit includes
// nothing else before it and uses every public type of the pipeline through it alone.
#include <treegen/TreeGenerator.h>

#include <doctest/doctest.h>

TEST_CASE("TreeGenerator.h alone declares the whole pipeline") {
	treegen::TreeParams params;
	params.axiom = "F(1)[+F(1)]F(1)";
	params.leafGrowthProb = 1.0f;

	treegen::GenerateOptions options;
	options.seed = 1;
	treegen::TreeModel model = treegen::generateTree(params, options);

	treegen::MeshData& trunk = model.geometry.trunk;
	treegen::GeometryStats stats = model.geometry.stats;
	treegen::MeshVertex vertex = trunk.vertices.front();
	treegen::Rng rng(1);

	CHECK(trunk.vertices.size() == 54);
	CHECK(stats.emittedSegments == 3);
	CHECK(model.geometry.leafTransforms.size() == 2);
	CHECK(vertex.uv.y == 0.0f);
	CHECK(rng.uniform01() < 1.0f);
	CHECK(!treegen::toString(model.modules).empty());
	CHECK_THROWS_AS(throw treegen::InterpretError("x"), treegen::Error);
}
