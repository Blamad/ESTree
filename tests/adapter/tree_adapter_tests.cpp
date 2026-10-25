// Headless tests of the engine's tree adapter: the pure conversion helpers (treegen output -> engine Vertex / int
// indices / InstancedTransform) and the seed rules. No window, no OpenGL context, no engine singletons: the helpers
// live in a header that only needs glm, the plain Vertex / InstancedTransform structs and treegen.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <treegen/TreeGenerator.h>

#include "ESTree/Models/Tree/TreeMeshConversion.h"

namespace fs = std::filesystem;

namespace {

const fs::path kRulesDir = fs::path(ESTREE_REPO_ROOT) / "Resources" / "LindenmayerRules";

bool closeTo(const glm::mat4 &a, const glm::mat4 &b, float epsilon = 1e-5f) {
	for (int column = 0; column < 4; ++column)
		for (int row = 0; row < 4; ++row)
			if (std::abs(a[column][row] - b[column][row]) > epsilon)
				return false;
	return true;
}

// What LindenmayerTree::generate() does with the treegen output, minus the GL upload.
struct Converted {
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	std::vector<InstancedTransform> leaves;
	uint32_t seed = 0;
};

Converted generateAndConvert(const treegen::TreeParams &params, uint32_t requestedSeed, const glm::mat4 &world = glm::mat4(1)) {
	Converted converted;
	converted.seed = treeadapter::chooseSeed(params.seed, requestedSeed);
	treegen::GenerateOptions options;
	options.seed = converted.seed;
	const treegen::TreeModel model = treegen::generateTree(params, options);
	converted.vertices = treeadapter::toEngineVertices(model.geometry.trunk.vertices);
	converted.indices = treeadapter::toEngineIndices(model.geometry.trunk.indices);
	converted.leaves = treeadapter::toInstancedTransforms(model.geometry.leafTransforms, world);
	return converted;
}

bool sameGeometry(const Converted &a, const Converted &b) {
	if (a.vertices.size() != b.vertices.size() || a.indices != b.indices || a.leaves.size() != b.leaves.size())
		return false;
	for (size_t i = 0; i < a.vertices.size(); ++i)
		if (a.vertices[i].position != b.vertices[i].position || a.vertices[i].normal != b.vertices[i].normal || a.vertices[i].texCoords != b.vertices[i].texCoords)
			return false;
	for (size_t i = 0; i < a.leaves.size(); ++i)
		if (a.leaves[i].modelMatrix != b.leaves[i].modelMatrix)
			return false;
	return true;
}

}

// ---- vertices -------------------------------------------------------------------------------------------

TEST_CASE("toEngineVertex: copies position, normal and uv and zeroes the color") {
	treegen::MeshVertex source;
	source.position = glm::vec3(1.0f, 2.0f, 3.0f);
	source.normal = glm::vec3(0.0f, 0.6f, 0.8f);
	source.uv = glm::vec2(0.25f, 1.0f);

	const Vertex vertex = treeadapter::toEngineVertex(source);
	CHECK(vertex.position == glm::vec3(1.0f, 2.0f, 3.0f));
	CHECK(vertex.normal == glm::vec3(0.0f, 0.6f, 0.8f));
	CHECK(vertex.texCoords == glm::vec2(0.25f, 1.0f));
	CHECK(vertex.color == glm::vec4(0.0f));
}

TEST_CASE("toEngineVertices: keeps count and order") {
	std::vector<treegen::MeshVertex> source(3);
	for (int i = 0; i < 3; ++i) {
		source[i].position = glm::vec3(float(i), 0, 0);
		source[i].normal = glm::vec3(0, float(i), 0);
		source[i].uv = glm::vec2(float(i), float(i) * 2.0f);
	}
	const std::vector<Vertex> vertices = treeadapter::toEngineVertices(source);
	REQUIRE(vertices.size() == 3);
	for (int i = 0; i < 3; ++i) {
		CHECK(vertices[i].position.x == float(i));
		CHECK(vertices[i].normal.y == float(i));
		CHECK(vertices[i].texCoords.y == float(i) * 2.0f);
	}
	CHECK(treeadapter::toEngineVertices({}).empty());
}

// ---- indices --------------------------------------------------------------------------------------------

TEST_CASE("toEngineIndex: values up to INT_MAX are kept") {
	CHECK(treeadapter::toEngineIndex(0u) == 0);
	CHECK(treeadapter::toEngineIndex(12345u) == 12345);
	CHECK(treeadapter::toEngineIndex(static_cast<uint32_t>(INT_MAX)) == INT_MAX);
}

TEST_CASE("toEngineIndex: values above INT_MAX are refused instead of wrapping to negative") {
	CHECK_THROWS_AS(treeadapter::toEngineIndex(static_cast<uint32_t>(INT_MAX) + 1u), std::length_error);
	CHECK_THROWS_AS(treeadapter::toEngineIndex(UINT32_MAX), std::length_error);
}

TEST_CASE("toEngineIndices: converts in order and refuses a list with one oversized index") {
	const std::vector<int> converted = treeadapter::toEngineIndices({ 0u, 1u, 2u, 2u, 1u, 3u });
	CHECK(converted == std::vector<int>({ 0, 1, 2, 2, 1, 3 }));
	CHECK(treeadapter::toEngineIndices({}).empty());
	CHECK_THROWS_AS(treeadapter::toEngineIndices({ 0u, 1u, 0x80000000u }), std::length_error);
}

TEST_CASE("checkedBufferSize: counts above INT_MAX are refused") {
	CHECK(treeadapter::checkedBufferSize(0, "vertex") == 0);
	CHECK(treeadapter::checkedBufferSize(2125746, "vertex") == 2125746);
	CHECK(treeadapter::checkedBufferSize(static_cast<size_t>(INT_MAX), "vertex") == INT_MAX);
	if (sizeof(size_t) > sizeof(uint32_t)) {
		const size_t tooMany = static_cast<size_t>(INT_MAX) + 1;
		CHECK_THROWS_AS(treeadapter::checkedBufferSize(tooMany, "vertex"), std::length_error);
	}
}

// ---- leaves ---------------------------------------------------------------------------------------------

TEST_CASE("toInstancedTransforms: the tree's world matrix is premultiplied, not postmultiplied") {
	const glm::mat4 world = glm::translate(glm::mat4(1), glm::vec3(10.0f, 0.0f, -4.0f)) * glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 1, 0));
	const glm::mat4 leafA = glm::translate(glm::mat4(1), glm::vec3(1.0f, 2.0f, 3.0f)) * glm::rotate(glm::mat4(1), glm::radians(30.0f), glm::vec3(1, 0, 0));
	const glm::mat4 leafB = glm::scale(glm::mat4(1), glm::vec3(2.0f));

	const std::vector<InstancedTransform> transforms = treeadapter::toInstancedTransforms({ leafA, leafB }, world);
	REQUIRE(transforms.size() == 2);
	CHECK(closeTo(transforms[0].modelMatrix, world * leafA));
	CHECK(closeTo(transforms[1].modelMatrix, world * leafB));
	CHECK_FALSE(closeTo(transforms[0].modelMatrix, leafA * world));

	// the leaf origin (1,2,3 rotated by 30 degrees about X stays at the translation) ends up at world * (1,2,3)
	const glm::vec4 origin = transforms[0].modelMatrix * glm::vec4(0, 0, 0, 1);
	const glm::vec4 expected = world * glm::vec4(1, 2, 3, 1);
	CHECK(origin.x == doctest::Approx(expected.x));
	CHECK(origin.y == doctest::Approx(expected.y));
	CHECK(origin.z == doctest::Approx(expected.z));
}

TEST_CASE("toInstancedTransforms: an identity world keeps the leaf matrices and no leaves give no instances") {
	const glm::mat4 leaf = glm::translate(glm::mat4(1), glm::vec3(1.0f, 2.0f, 3.0f));
	const std::vector<InstancedTransform> transforms = treeadapter::toInstancedTransforms({ leaf }, glm::mat4(1));
	REQUIRE(transforms.size() == 1);
	CHECK(transforms[0].modelMatrix == leaf);
	CHECK(treeadapter::toInstancedTransforms({}, leaf).empty());
}

// ---- seeds ----------------------------------------------------------------------------------------------

TEST_CASE("chooseSeed: the rule file's seed wins over the console seed") {
	int fresh = 0;
	const auto counter = [&fresh]() { ++fresh; return 99u; };
	CHECK(treeadapter::chooseSeed(7, 42, counter) == 7);
	CHECK(treeadapter::chooseSeed(7, 0, counter) == 7);
	CHECK(fresh == 0);
}

TEST_CASE("chooseSeed: the console seed is used when the file has none") {
	int fresh = 0;
	const auto counter = [&fresh]() { ++fresh; return 99u; };
	CHECK(treeadapter::chooseSeed(0, 42, counter) == 42);
	CHECK(fresh == 0);
}

TEST_CASE("chooseSeed: a fresh seed is drawn only when neither is given, and is never 0") {
	int fresh = 0;
	const auto counter = [&fresh]() { ++fresh; return 99u; };
	CHECK(treeadapter::chooseSeed(0, 0, counter) == 99);
	CHECK(fresh == 1);
	CHECK(treeadapter::chooseSeed(0, 0, []() { return 0u; }) == 1);
	for (int i = 0; i < 200; ++i)
		CHECK(treeadapter::randomSeed() != 0);
	std::set<uint32_t> distinct;
	for (int i = 0; i < 20; ++i)
		distinct.insert(treeadapter::chooseSeed(0, 0));
	CHECK(distinct.size() > 1);
}

// ---- the adapter path -----------------------------------------------------------------------------------

TEST_CASE("adapter path: tree file 15 with seed 42 twice gives identical counts and data") {
	const treegen::TreeParams params = treegen::loadTreeParams(kRulesDir / "parametricTernaryTreeB.json");
	const Converted first = generateAndConvert(params, 42);
	const Converted second = generateAndConvert(params, 42);
	CHECK(first.seed == 42);
	CHECK(first.vertices.size() == second.vertices.size());
	CHECK(first.indices.size() == second.indices.size());
	CHECK(first.leaves.size() == second.leaves.size());
	CHECK(sameGeometry(first, second));
	CHECK(first.vertices.size() == 26244);
	CHECK(first.indices.size() / 3 == 23328); // 1458 segments of 16 triangles: the "tris" the engine logs for this tree
	CHECK(first.indices.size() % 3 == 0);
	for (int index : first.indices) {
		REQUIRE(index >= 0);
		REQUIRE(static_cast<size_t>(index) < first.vertices.size());
	}
}

TEST_CASE("adapter path: a stochastic file differs between seeds and repeats for one seed") {
	const treegen::TreeParams params = treegen::loadTreeParams(kRulesDir / "advancedTree2.json");
	const Converted a = generateAndConvert(params, 42);
	const Converted again = generateAndConvert(params, 42);
	const Converted b = generateAndConvert(params, 43);
	CHECK(sameGeometry(a, again));
	CHECK_FALSE(sameGeometry(a, b));
}

TEST_CASE("adapter path: a seed in the rule file overrides the console seed") {
	treegen::TreeParams params = treegen::loadTreeParams(kRulesDir / "advancedTree2.json");
	params.seed = 5;
	const Converted fromFile = generateAndConvert(params, 42);
	CHECK(fromFile.seed == 5);
	params.seed = 0;
	const Converted five = generateAndConvert(params, 5);
	CHECK(sameGeometry(fromFile, five));
}

TEST_CASE("adapter path: the world transform moves the leaves, not the trunk") {
	const treegen::TreeParams params = treegen::loadTreeParams(kRulesDir / "parametricSympodialTreeA.json");
	const glm::mat4 world = glm::translate(glm::mat4(1), glm::vec3(20.0f, 0.0f, 20.0f));
	const Converted atOrigin = generateAndConvert(params, 3);
	const Converted moved = generateAndConvert(params, 3, world);
	REQUIRE(atOrigin.leaves.size() == moved.leaves.size());
	REQUIRE_FALSE(moved.leaves.empty());
	CHECK(atOrigin.vertices.size() == moved.vertices.size());
	for (size_t i = 0; i < moved.leaves.size(); ++i)
		CHECK(closeTo(moved.leaves[i].modelMatrix, world * atOrigin.leaves[i].modelMatrix));
}

TEST_CASE("adapter path: every shipped rule file at depth min(depth, 6) converts without error") {
	std::vector<fs::path> files;
	for (const fs::path &dir : { kRulesDir, kRulesDir / "legacy" })
		for (const fs::directory_entry &entry : fs::directory_iterator(dir))
			if (entry.is_regular_file() && (entry.path().extension() == ".json" || entry.path().extension() == ".l"))
				files.push_back(entry.path());
	std::sort(files.begin(), files.end());
	REQUIRE(files.size() == 43);

	for (const fs::path &file : files) {
		CAPTURE(file.string());
		treegen::TreeParams params = treegen::loadTreeParams(file);
		params.depth = std::min(params.depth, 6);
		Converted converted;
		REQUIRE_NOTHROW(converted = generateAndConvert(params, 12345));
		CHECK(converted.seed == 12345);
		CHECK(converted.indices.size() % 3 == 0);
		const size_t vertexCount = converted.vertices.size();
		CHECK(std::all_of(converted.indices.begin(), converted.indices.end(), [vertexCount](int index) { return index >= 0 && static_cast<size_t>(index) < vertexCount; }));
	}
}

TEST_CASE("adapter path: the deepest shipped tree (ternary E, depth 10) needs buffers far beyond the old caps") {
	treegen::TreeParams params = treegen::loadTreeParams(kRulesDir / "parametricTernaryTreeE.json");
	params.depth = 10;
	const Converted converted = generateAndConvert(params, 1);
	CHECK(converted.vertices.size() > 2000000); // old cap: 100 000
	CHECK(converted.indices.size() > 4000000);  // old cap: 4 000 000
	CHECK_NOTHROW(treeadapter::checkedBufferSize(converted.vertices.size(), "vertex"));
	CHECK_NOTHROW(treeadapter::checkedBufferSize(converted.indices.size(), "index"));
	CHECK(converted.leaves.size() > 50000);
}
