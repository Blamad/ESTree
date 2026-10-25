#include <doctest/doctest.h>

#include <cstdint>
#include <vector>

#include <treegen/Rng.h>

namespace {

std::vector<float> draw(treegen::Rng rng, int count) {
	std::vector<float> values;
	for (int i = 0; i < count; ++i)
		values.push_back(rng.uniform01());
	return values;
}

}

TEST_CASE("Rng: the same seed gives the same sequence") {
	CHECK(draw(treegen::Rng(42), 64) == draw(treegen::Rng(42), 64));
}

TEST_CASE("Rng: different seeds give different sequences") {
	CHECK(draw(treegen::Rng(1), 16) != draw(treegen::Rng(2), 16));
}

TEST_CASE("Rng: uniform01 stays in the half-open unit interval and is not constant") {
	treegen::Rng rng(7);
	float lowest = 1.0f, highest = 0.0f;
	for (int i = 0; i < 10000; ++i) {
		float v = rng.uniform01();
		REQUIRE(v >= 0.0f);
		REQUIRE(v < 1.0f);
		lowest = v < lowest ? v : lowest;
		highest = v > highest ? v : highest;
	}
	CHECK(lowest < 0.05f);
	CHECK(highest > 0.95f);
}

TEST_CASE("Rng: values are fixed by the seed (cross-platform contract)") {
	// mt19937(1) first output is 1791095845; uniform01 keeps its top 24 bits: 1791095845 >> 8 = 6996468.
	treegen::Rng rng(1);
	CHECK(rng.uniform01() == 6996468.0f / 16777216.0f);
}

TEST_CASE("Rng: derive gives distinct, reproducible streams") {
	treegen::Rng root(123);
	auto rewrite = draw(root.derive(1), 16);
	auto turtle = draw(root.derive(2), 16);
	auto leaves = draw(root.derive(3), 16);
	CHECK(rewrite != turtle);
	CHECK(turtle != leaves);
	CHECK(rewrite != leaves);
	CHECK(rewrite != draw(root, 16));
	CHECK(rewrite == draw(treegen::Rng(123).derive(1), 16));
}

TEST_CASE("Rng: derive does not depend on draws already taken from the parent") {
	treegen::Rng a(5), b(5);
	for (int i = 0; i < 100; ++i)
		a.uniform01();
	CHECK(draw(a.derive(2), 16) == draw(b.derive(2), 16));
}

TEST_CASE("Rng: drawing from a derived stream does not move the parent") {
	treegen::Rng parent(9);
	auto expected = draw(treegen::Rng(9), 8);
	treegen::Rng child = parent.derive(1);
	child.uniform01();
	CHECK(draw(parent, 8) == expected);
}

TEST_CASE("Rng: derived streams of different parents differ") {
	CHECK(draw(treegen::Rng(1).derive(1), 16) != draw(treegen::Rng(2).derive(1), 16));
}
