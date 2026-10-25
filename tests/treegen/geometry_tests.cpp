// treegen::buildGeometry: exact counts, ring layout, UVs, normals, determinism, thread independence, leaves,
// degenerate input. Parity with the legacy generator is in geometry_parity_tests.cpp, the facade in
// geometry_facade_tests.cpp.
#include <doctest/doctest.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <limits>
#include <string>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include <treegen/Error.h>
#include <treegen/Geometry.h>
#include <treegen/Module.h>
#include <treegen/TreeGenerator.h>
#include <treegen/TreeParams.h>
#include <treegen/Turtle.h>

namespace {

const float kPi = 3.14159265358979f;

treegen::TreeParams defaultParams() {
	treegen::TreeParams p;
	p.leafGrowthProb = 1.0f;
	p.leavesAngleDiversity = 0.0f;
	return p;
}

treegen::Skeleton fromProduction(const std::string& production, const treegen::TreeParams& params = defaultParams()) {
	return treegen::interpret(params, treegen::parseModules(production));
}

// A straight chain of `n` segments along +Y, each `step` long.
treegen::Skeleton chain(int n, float radius = 1.0f, float step = 1.0f) {
	treegen::Skeleton s;
	s.segments.push_back({ -1, glm::mat4(1.0f), radius, 0.0f, n > 0 ? 1 : 0 });
	for (int i = 1; i <= n; ++i)
		s.segments.push_back({ i - 1, glm::translate(glm::mat4(1.0f), glm::vec3(0, step * i, 0)), radius, 0.0f, i < n ? 1 : 0 });
	return s;
}

// A binary "heap" of `n` non-root segments with varying angles, rolls and radii: parent(i) = (i - 1) / 2.
treegen::Skeleton bushy(int n) {
	treegen::Skeleton s;
	s.segments.push_back({ -1, glm::mat4(1.0f), 1.0f, 0.0f, 0 });
	for (int i = 1; i <= n; ++i) {
		int parent = (i - 1) / 2;
		float roll = 0.3f * float(i % 7) - 0.8f;
		glm::mat4 local = glm::rotate(glm::mat4(1.0f), 0.2f + 0.05f * float(i % 5), glm::vec3(1, 0, 0));
		local = glm::rotate(local, roll, glm::vec3(0, 1, 0));
		local = glm::translate(local, glm::vec3(0, 1.0f / (1.0f + 0.01f * float(i % 11)), 0));
		treegen::SkeletonSegment seg{ parent, s.segments[parent].model * local, 1.0f / (1.0f + 0.02f * float(i % 13)), s.segments[parent].roll + roll, 0 };
		s.segments[parent].childCount++;
		s.segments.push_back(seg);
	}
	return s;
}

size_t countNonFinite(const treegen::TreeGeometry& g) {
	size_t bad = 0;
	for (const treegen::MeshVertex& v : g.trunk.vertices) {
		const float* f = &v.position.x;
		for (int i = 0; i < 8; ++i)
			if (!std::isfinite(f[i]))
				++bad;
	}
	for (const glm::mat4& m : g.leafTransforms)
		for (int c = 0; c < 4; ++c)
			for (int r = 0; r < 4; ++r)
				if (!std::isfinite(m[c][r]))
					++bad;
	return bad;
}

bool sameBytes(const treegen::TreeGeometry& a, const treegen::TreeGeometry& b) {
	return a.trunk.vertices.size() == b.trunk.vertices.size()
		&& a.trunk.indices == b.trunk.indices
		&& a.leafTransforms.size() == b.leafTransforms.size()
		&& (a.trunk.vertices.empty() || std::memcmp(a.trunk.vertices.data(), b.trunk.vertices.data(), a.trunk.vertices.size() * sizeof(treegen::MeshVertex)) == 0)
		&& (a.leafTransforms.empty() || std::memcmp(a.leafTransforms.data(), b.leafTransforms.data(), a.leafTransforms.size() * sizeof(glm::mat4)) == 0);
}

size_t countTips(const treegen::Skeleton& s) {
	size_t n = 0;
	for (const treegen::SkeletonSegment& seg : s.segments)
		if (treegen::isTip(seg))
			++n;
	return n;
}

}

TEST_CASE("buildGeometry: a single F(1) gives 18 vertices and 48 indices") {
	treegen::TreeParams params = defaultParams();
	treegen::TreeGeometry g = treegen::buildGeometry(fromProduction("F(1)", params), params);

	CHECK(g.trunk.vertices.size() == 18);
	CHECK(g.trunk.indices.size() == 48);   // 8 quads x 6 indices = 16 triangles
	CHECK(g.stats.segments == 1);
	CHECK(g.stats.emittedSegments == 1);
	CHECK(g.stats.skippedZeroRadius == 0);
	CHECK(g.stats.skippedInvalidFrame == 0);

	// stored radius 1 is a diameter: the ring has geometric radius 0.5, bottom at y = 0, top at y = 1
	for (int j = 0; j < 9; ++j) {
		float t = 2.0f * kPi * float(j) / 8.0f;
		const treegen::MeshVertex& bottom = g.trunk.vertices[j];
		const treegen::MeshVertex& top = g.trunk.vertices[9 + j];
		CHECK(bottom.position.x == doctest::Approx(0.5f * std::cos(t)).epsilon(1e-5));
		CHECK(bottom.position.z == doctest::Approx(0.5f * std::sin(t)).epsilon(1e-5));
		CHECK(bottom.position.y == doctest::Approx(0.0f).epsilon(1e-6));
		CHECK(top.position.y == doctest::Approx(1.0f).epsilon(1e-6));
		CHECK(top.position.x == doctest::Approx(bottom.position.x).epsilon(1e-6));
	}
}

TEST_CASE("buildGeometry: counts are exact for chains and ring densities") {
	treegen::TreeParams params = defaultParams();
	for (int rd : { 3, 5, 9, 16 })
		for (int n : { 1, 2, 5, 20 }) {
			treegen::GeometryOptions options;
			options.ringDensity = rd;
			treegen::TreeGeometry g = treegen::buildGeometry(chain(n), params, options);
			INFO("ringDensity " << rd << " segments " << n);
			CHECK(g.trunk.vertices.size() == size_t(2 * rd * n));
			CHECK(g.trunk.indices.size() == size_t(6 * (rd - 1) * n));
			CHECK(g.trunk.indices.size() % 3 == 0);
			CHECK(g.stats.emittedSegments == size_t(n));
		}
}

TEST_CASE("buildGeometry: the root alone and an empty skeleton emit nothing") {
	treegen::TreeParams params = defaultParams();

	treegen::TreeGeometry rootOnly = treegen::buildGeometry(fromProduction("", params), params);
	CHECK(rootOnly.trunk.vertices.empty());
	CHECK(rootOnly.trunk.indices.empty());
	CHECK(rootOnly.stats.segments == 0);

	treegen::TreeGeometry empty = treegen::buildGeometry(treegen::Skeleton(), params);
	CHECK(empty.trunk.vertices.empty());
	CHECK(empty.trunk.indices.empty());
	CHECK(empty.leafTransforms.empty());
}

TEST_CASE("buildGeometry: UVs run u 0..1 along a ring and v 0 (bottom) / 1 (top)") {
	treegen::TreeParams params = defaultParams();
	for (int rd : { 3, 7, 9 }) {
		treegen::GeometryOptions options;
		options.ringDensity = rd;
		treegen::TreeGeometry g = treegen::buildGeometry(chain(3), params, options);
		for (size_t i = 0; i < g.trunk.vertices.size(); ++i) {
			size_t ring = i / size_t(rd);
			int j = int(i % size_t(rd));
			const glm::vec2& uv = g.trunk.vertices[i].uv;
			CHECK(uv.x >= 0.0f);
			CHECK(uv.x <= 1.0f + 1e-6f);
			CHECK(uv.x == doctest::Approx(float(j) / float(rd - 1)).epsilon(1e-6));
			CHECK(uv.y == (ring % 2 == 0 ? 0.0f : 1.0f));
		}
	}
}

TEST_CASE("buildGeometry: the last ring vertex duplicates the first except for u") {
	treegen::TreeParams params = defaultParams();
	treegen::TreeGeometry g = treegen::buildGeometry(fromProduction("F(2)+(30)F(1)^(20)F(1)", params), params);
	REQUIRE(!g.trunk.vertices.empty());
	for (size_t ring = 0; ring * 9 < g.trunk.vertices.size(); ++ring) {
		const treegen::MeshVertex& first = g.trunk.vertices[ring * 9];
		const treegen::MeshVertex& last = g.trunk.vertices[ring * 9 + 8];
		CHECK(glm::length(first.position - last.position) < 1e-5f);
		CHECK(glm::length(first.normal - last.normal) < 1e-5f);
		CHECK(first.uv.x == 0.0f);
		CHECK(last.uv.x == doctest::Approx(1.0f).epsilon(1e-6));
		CHECK(first.uv.y == last.uv.y);
	}
}

TEST_CASE("buildGeometry: normals are unit length and radial") {
	treegen::TreeParams params = defaultParams();
	treegen::TreeGeometry g = treegen::buildGeometry(fromProduction("F(1)[+(40)^(30)F(1)][-(40)<(25)F(2)]F(1)", params), params);
	REQUIRE(!g.trunk.vertices.empty());
	for (const treegen::MeshVertex& v : g.trunk.vertices)
		CHECK(std::fabs(glm::length(v.normal) - 1.0f) < 1e-4f);

	// straight chain: the normal is the horizontal direction from the axis to the vertex
	treegen::TreeGeometry straight = treegen::buildGeometry(chain(2), params);
	for (const treegen::MeshVertex& v : straight.trunk.vertices) {
		glm::vec3 radial = glm::normalize(glm::vec3(v.position.x, 0, v.position.z));
		CHECK(glm::length(v.normal - radial) < 1e-4f);
	}
}

TEST_CASE("buildGeometry: indices stay below the vertex count and follow the legacy quad order") {
	treegen::TreeParams params = defaultParams();
	treegen::TreeGeometry g = treegen::buildGeometry(fromProduction("F(1)[+(40)F(1)][-(40)F(2)]F(1)", params), params);
	for (uint32_t index : g.trunk.indices)
		CHECK(index < g.trunk.vertices.size());

	// segment k owns vertices [18k, 18k + 18): bottom ring b = 18k, top ring t = b + 9; per quad i:
	// (b+i, t+i, b+i+1) and (t+i, t+i+1, b+i+1)
	const size_t segments = g.trunk.vertices.size() / 18;
	REQUIRE(g.trunk.indices.size() == segments * 48);
	for (size_t k = 0; k < segments; ++k)
		for (uint32_t i = 0; i < 8; ++i) {
			uint32_t b = uint32_t(18 * k), t = b + 9;
			const uint32_t* q = &g.trunk.indices[k * 48 + i * 6];
			CHECK(q[0] == b + i);
			CHECK(q[1] == t + i);
			CHECK(q[2] == b + i + 1);
			CHECK(q[3] == t + i);
			CHECK(q[4] == t + i + 1);
			CHECK(q[5] == b + i + 1);
		}
}

TEST_CASE("buildGeometry: a child's bottom ring continues the parent's top ring when the rolls agree") {
	treegen::TreeParams params = defaultParams();
	treegen::TreeGeometry g = treegen::buildGeometry(chain(2, 1.0f, 1.0f), params);
	REQUIRE(g.trunk.vertices.size() == 36);
	for (int j = 0; j < 9; ++j)
		CHECK(glm::length(g.trunk.vertices[9 + j].position - g.trunk.vertices[18 + j].position) < 1e-5f);
}

TEST_CASE("buildGeometry: the bottom ring is turned about Y by the roll difference") {
	treegen::TreeParams params = defaultParams();
	treegen::Skeleton s = chain(1);
	s.segments[1].roll = kPi / 2;   // parent roll 0
	treegen::TreeGeometry g = treegen::buildGeometry(s, params);
	// vertex 0 of the unrotated ring is (0.5, 0, 0); a quarter turn about +Y takes it to (0, 0, -0.5)
	CHECK(glm::length(g.trunk.vertices[0].position - glm::vec3(0, 0, -0.5f)) < 1e-5f);
	// the top ring is not rotated
	CHECK(glm::length(g.trunk.vertices[9].position - glm::vec3(0.5f, 1, 0)) < 1e-5f);
}

TEST_CASE("buildGeometry: ringDensity below 3 and broken parent links throw") {
	treegen::TreeParams params = defaultParams();
	treegen::GeometryOptions options;
	for (int rd : { -1, 0, 1, 2 }) {
		options.ringDensity = rd;
		CHECK_THROWS_AS(treegen::buildGeometry(chain(2), params, options), treegen::Error);
	}

	treegen::Skeleton badParent = chain(2);
	badParent.segments[2].parent = 2;   // itself
	CHECK_THROWS_AS(treegen::buildGeometry(badParent, params), treegen::Error);
	badParent.segments[2].parent = 7;
	CHECK_THROWS_AS(treegen::buildGeometry(badParent, params), treegen::Error);
	badParent.segments[2].parent = -1;
	CHECK_THROWS_AS(treegen::buildGeometry(badParent, params), treegen::Error);
}

// ---------------------------------------------------------------------------------------------------------------
// Determinism and threads

TEST_CASE("buildGeometry: the same input gives identical bytes") {
	treegen::TreeParams params = defaultParams();
	params.leafGrowthProb = 0.5f;
	params.leavesAngleDiversity = 0.6f;
	treegen::Skeleton s = bushy(400);
	treegen::GeometryOptions options;
	options.seed = 99;
	treegen::TreeGeometry a = treegen::buildGeometry(s, params, options);
	treegen::TreeGeometry b = treegen::buildGeometry(s, params, options);
	CHECK(sameBytes(a, b));
	CHECK(!a.leafTransforms.empty());
}

TEST_CASE("buildGeometry: thread count does not change a single bit") {
	treegen::TreeParams params = defaultParams();
	params.leafGrowthProb = 0.7f;
	params.leavesAngleDiversity = 0.4f;

	SUBCASE("a mesh above the automatic threshold (parallel when threads = 0)") {
		treegen::Skeleton s = bushy(4000);   // 72 000 vertices
		treegen::GeometryOptions options;
		options.seed = 5;
		options.threads = 1;
		treegen::TreeGeometry single = treegen::buildGeometry(s, params, options);
		REQUIRE(single.trunk.vertices.size() > 50000);
		for (unsigned threads : { 0u, 2u, 3u, 4u, 7u, 64u }) {
			options.threads = threads;
			INFO("threads " << threads);
			CHECK(sameBytes(single, treegen::buildGeometry(s, params, options)));
		}
	}
	SUBCASE("a small mesh with the multi-thread path forced") {
		treegen::Skeleton s = bushy(37);
		treegen::GeometryOptions options;
		options.seed = 5;
		options.threads = 1;
		treegen::TreeGeometry single = treegen::buildGeometry(s, params, options);
		for (unsigned threads : { 2u, 3u, 8u, 100u }) {
			options.threads = threads;
			INFO("threads " << threads);
			CHECK(sameBytes(single, treegen::buildGeometry(s, params, options)));
		}
	}
	SUBCASE("more threads than segments, one segment") {
		treegen::GeometryOptions options;
		options.threads = 1;
		treegen::TreeGeometry single = treegen::buildGeometry(chain(1), params, options);
		options.threads = 16;
		CHECK(sameBytes(single, treegen::buildGeometry(chain(1), params, options)));
	}
}

// ---------------------------------------------------------------------------------------------------------------
// Leaves

TEST_CASE("buildGeometry leaves: one per tip with leafGrowthProb 1, and the tip's own frame") {
	treegen::TreeParams params = defaultParams();
	treegen::Skeleton s = fromProduction("F(1)[+(40)F(1)][-(40)F(2)][^(30)F(1)]F(1)", params);
	treegen::TreeGeometry g = treegen::buildGeometry(s, params);
	REQUIRE(countTips(s) == 4);
	REQUIRE(g.leafTransforms.size() == 4);
	size_t leaf = 0;
	for (const treegen::SkeletonSegment& seg : s.segments)
		if (treegen::isTip(seg))
			CHECK(g.leafTransforms[leaf++] == seg.model);   // diversity 0: no rotation, exactly the segment frame
}

TEST_CASE("buildGeometry leaves: leafGrowthProb <= 0 gives none") {
	treegen::TreeParams params = defaultParams();
	treegen::Skeleton s = bushy(50);
	for (float prob : { 0.0f, -1.0f, -0.0f }) {
		params.leafGrowthProb = prob;
		CHECK(treegen::buildGeometry(s, params).leafTransforms.empty());
	}
	params.leafGrowthProb = std::numeric_limits<float>::quiet_NaN();
	CHECK(treegen::buildGeometry(s, params).leafTransforms.empty());
}

TEST_CASE("buildGeometry leaves: a probability of one half keeps about half, depending on the seed") {
	treegen::TreeParams params = defaultParams();
	params.leafGrowthProb = 0.5f;
	treegen::Skeleton s = bushy(801);   // ~400 tips
	const size_t tips = countTips(s);
	REQUIRE(tips > 300);

	treegen::GeometryOptions a, b;
	a.seed = 1;
	b.seed = 2;
	treegen::TreeGeometry ga = treegen::buildGeometry(s, params, a);
	treegen::TreeGeometry ga2 = treegen::buildGeometry(s, params, a);
	treegen::TreeGeometry gb = treegen::buildGeometry(s, params, b);

	CHECK(ga.leafTransforms == ga2.leafTransforms);
	CHECK(ga.leafTransforms != gb.leafTransforms);   // a different leaf set
	CHECK(ga.leafTransforms.size() > tips * 35 / 100);
	CHECK(ga.leafTransforms.size() < tips * 65 / 100);
	CHECK(gb.leafTransforms.size() > tips * 35 / 100);
	CHECK(gb.leafTransforms.size() < tips * 65 / 100);

	// the trunk does not depend on the leaf seed
	CHECK(ga.trunk.indices == gb.trunk.indices);
	CHECK(ga.trunk.vertices.size() == gb.trunk.vertices.size());
	CHECK(std::memcmp(ga.trunk.vertices.data(), gb.trunk.vertices.data(), ga.trunk.vertices.size() * sizeof(treegen::MeshVertex)) == 0);
}

TEST_CASE("buildGeometry leaves: the angle diversity is a rotation about Y within plus/minus d") {
	treegen::TreeParams params = defaultParams();
	params.leavesAngleDiversity = 0.5f;
	treegen::Skeleton s = bushy(301);
	treegen::TreeGeometry g = treegen::buildGeometry(s, params);

	REQUIRE(g.leafTransforms.size() == countTips(s));
	size_t leaf = 0;
	float lowest = 10, highest = -10;
	for (const treegen::SkeletonSegment& seg : s.segments) {
		if (!treegen::isTip(seg))
			continue;
		glm::mat4 rotation = glm::inverse(seg.model) * g.leafTransforms[leaf++];
		float angle = std::atan2(rotation[2][0], rotation[0][0]);   // rotation about Y by a: x' = x cos a + z sin a
		CHECK(std::fabs(rotation[1][1] - 1.0f) < 1e-4f);
		CHECK(std::fabs(rotation[3][0]) < 1e-4f);
		CHECK(std::fabs(angle) <= 0.5f + 1e-4f);
		lowest = std::min(lowest, angle);
		highest = std::max(highest, angle);
	}
	CHECK(lowest < -0.3f);   // the angles really spread over the range
	CHECK(highest > 0.3f);
}

// ---------------------------------------------------------------------------------------------------------------
// Degenerate input (Review Focus 5, geometry half)

TEST_CASE("buildGeometry: zero or negative radius segments are skipped") {
	treegen::TreeParams params = defaultParams();
	// root(1) <- s1(1) <- s2(0) <- s3(1): s2 has radius 0, s3 has a zero-radius bottom ring
	treegen::Skeleton s = chain(3);
	s.segments[2].radius = 0.0f;
	treegen::TreeGeometry g = treegen::buildGeometry(s, params);
	CHECK(g.trunk.vertices.size() == 18);
	CHECK(g.trunk.indices.size() == 48);
	CHECK(g.stats.segments == 3);
	CHECK(g.stats.emittedSegments == 1);
	CHECK(g.stats.skippedZeroRadius == 2);
	CHECK(g.stats.skippedInvalidFrame == 0);
	CHECK(countNonFinite(g) == 0);
	for (uint32_t index : g.trunk.indices)
		CHECK(index < g.trunk.vertices.size());

	s = chain(2);
	s.segments[1].radius = -1.0f;
	g = treegen::buildGeometry(s, params);
	CHECK(g.trunk.vertices.empty());
	CHECK(g.stats.skippedZeroRadius == 2);

	s = chain(1);
	s.segments[0].radius = 0.0f;   // the root's radius is the bottom ring of the first segments
	g = treegen::buildGeometry(s, params);
	CHECK(g.trunk.vertices.empty());
	CHECK(g.stats.skippedZeroRadius == 1);
}

TEST_CASE("buildGeometry: non-finite radius, frame or roll skips the segment and its children") {
	treegen::TreeParams params = defaultParams();
	const float nan = std::numeric_limits<float>::quiet_NaN();
	const float inf = std::numeric_limits<float>::infinity();

	SUBCASE("NaN in the model") {
		treegen::Skeleton s = chain(3);
		s.segments[2].model[3][1] = nan;
		treegen::TreeGeometry g = treegen::buildGeometry(s, params);
		CHECK(g.stats.emittedSegments == 1);
		CHECK(g.stats.skippedInvalidFrame == 2);
		CHECK(g.trunk.vertices.size() == 18);
		CHECK(countNonFinite(g) == 0);
	}
	SUBCASE("infinite model entry") {
		treegen::Skeleton s = chain(2);
		s.segments[1].model[0][0] = inf;
		treegen::TreeGeometry g = treegen::buildGeometry(s, params);
		CHECK(g.trunk.vertices.empty());
		CHECK(g.stats.skippedInvalidFrame == 2);
		CHECK(countNonFinite(g) == 0);
	}
	SUBCASE("NaN and infinite radius") {
		for (float radius : { nan, inf }) {
			treegen::Skeleton s = chain(2);
			s.segments[2].radius = radius;
			treegen::TreeGeometry g = treegen::buildGeometry(s, params);
			CHECK(g.stats.emittedSegments == 1);
			CHECK(g.stats.skippedInvalidFrame == 1);
			CHECK(countNonFinite(g) == 0);
		}
	}
	SUBCASE("NaN roll") {
		treegen::Skeleton s = chain(2);
		s.segments[2].roll = nan;
		treegen::TreeGeometry g = treegen::buildGeometry(s, params);
		CHECK(g.stats.skippedInvalidFrame == 1);
		CHECK(countNonFinite(g) == 0);
	}
	SUBCASE("entries so large that the normal would overflow") {
		treegen::Skeleton s = chain(2);
		s.segments[2].model = glm::mat4(1e30f);
		treegen::TreeGeometry g = treegen::buildGeometry(s, params);
		CHECK(g.stats.skippedInvalidFrame == 1);
		CHECK(countNonFinite(g) == 0);
	}
	SUBCASE("a singular frame (all-zero rotation part)") {
		treegen::Skeleton s = chain(2);
		s.segments[2].model = glm::mat4(0.0f);
		s.segments[2].model[3][3] = 1.0f;
		treegen::TreeGeometry g = treegen::buildGeometry(s, params);
		CHECK(g.stats.emittedSegments == 1);
		CHECK(g.stats.skippedInvalidFrame == 1);
		CHECK(countNonFinite(g) == 0);
	}
}

TEST_CASE("buildGeometry: tiny radii and frames still give finite unit normals") {
	treegen::TreeParams params = defaultParams();
	treegen::Skeleton s = chain(2, 1e-30f);
	treegen::TreeGeometry g = treegen::buildGeometry(s, params);
	REQUIRE(g.trunk.vertices.size() == 36);
	CHECK(countNonFinite(g) == 0);
	for (const treegen::MeshVertex& v : g.trunk.vertices)
		CHECK(std::fabs(glm::length(v.normal) - 1.0f) < 1e-4f);
}

TEST_CASE("buildGeometry: a leaf on a non-finite frame is dropped and counted") {
	treegen::TreeParams params = defaultParams();
	treegen::Skeleton s = chain(2);
	s.segments[2].model[3][2] = std::numeric_limits<float>::quiet_NaN();
	treegen::TreeGeometry g = treegen::buildGeometry(s, params);
	CHECK(g.leafTransforms.empty());
	CHECK(g.stats.skippedLeaves == 1);
	CHECK(countNonFinite(g) == 0);
}

TEST_CASE("buildGeometry: rules that produce zero length, zero radius or colliding segments stay finite") {
	treegen::TreeParams params = defaultParams();
	for (const char* production : {
		"F(0)F(0)F(0)",
		"=(0)F(1)F(1)",
		"`(0)F(1)F(1)F(1)",
		"F(1)[F(0)][F(0)]F(1)",
		"F(1)$F(1)$$F(1)",
		"T(0)F(1)T(3)F(1)",
		"F(1)+(0)^(0)<(0)F(1)",
	}) {
		INFO(production);
		treegen::TreeGeometry g = treegen::buildGeometry(fromProduction(production, params), params);
		CHECK(countNonFinite(g) == 0);
		for (uint32_t index : g.trunk.indices)
			CHECK(index < g.trunk.vertices.size());
	}
}

// ---------------------------------------------------------------------------------------------------------------
// Every shipped rule file

TEST_CASE("buildGeometry: NaN scan over every shipped rule file at depth min(file depth, 5)") {
	namespace fs = std::filesystem;
	const fs::path rulesDir = fs::path(ESTREE_REPO_ROOT) / "Resources" / "LindenmayerRules";

	std::vector<fs::path> files;
	for (const fs::directory_entry& entry : fs::recursive_directory_iterator(rulesDir))
		if (entry.is_regular_file() && (entry.path().extension() == ".json" || entry.path().extension() == ".l"))
			files.push_back(entry.path());
	std::sort(files.begin(), files.end());
	REQUIRE(files.size() >= 40);

	size_t generated = 0, failed = 0;
	std::vector<std::string> failures;
	size_t totalVertices = 0;
	for (const fs::path& file : files) {
		try {
			treegen::TreeParams params = treegen::loadTreeParams(file);
			params.depth = std::min(params.depth, 5);
			treegen::GenerateOptions options;
			options.seed = 12345;
			treegen::TreeModel model = treegen::generateTree(params, options);

			INFO(file.string());
			CHECK(countNonFinite(model.geometry) == 0);
			CHECK(model.geometry.trunk.indices.size() % 3 == 0);
			const size_t vertexCount = model.geometry.trunk.vertices.size();
			CHECK(std::all_of(model.geometry.trunk.indices.begin(), model.geometry.trunk.indices.end(), [vertexCount](uint32_t index) { return index < vertexCount; }));
			CHECK(model.geometry.stats.emittedSegments + model.geometry.stats.skippedZeroRadius + model.geometry.stats.skippedInvalidFrame == model.geometry.stats.segments);
			totalVertices += model.geometry.trunk.vertices.size();
			++generated;
		} catch (const treegen::Error& error) {
			++failed;
			failures.push_back(file.filename().string() + ": " + error.what());
		}
	}

	// Every shipped file must generate (legacy/parametricTree2.l lost a '(' until T07, see D-017); a new failure
	// here is a regression, not something to hide.
	INFO("failures: " << (failures.empty() ? std::string("none") : failures.front()));
	CHECK(failed == 0);
	CHECK(generated == files.size());
	CHECK(totalVertices > 100000);
}
