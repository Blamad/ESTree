#include <doctest/doctest.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

#include <treegen/Error.h>
#include <treegen/Module.h>
#include <treegen/Rng.h>
#include <treegen/Skeleton.h>
#include <treegen/Turtle.h>

using treegen::Skeleton;
using treegen::TreeParams;

namespace {

const float kPi = 3.14159265358979f;
const float kEps = 1e-4f;

// 90 degree default angle keeps the expected vectors exact: +Y turns into +Z (pitch), -X (yaw) and so on.
TreeParams makeParams() {
	TreeParams p;
	p.initialLength = 1.0f;
	p.initialRadius = 1.0f;
	p.angle = kPi / 2;
	p.tropism = glm::vec3(0, -1, 0);
	p.tropismBendingFactor = 0.2f;
	return p;
}

Skeleton run(const std::string& text, const TreeParams& params, uint32_t seed = 0) {
	treegen::InterpretOptions options;
	options.seed = seed;
	return treegen::interpret(params, treegen::parseModules(text), options);
}

Skeleton run(const std::string& text) { return run(text, makeParams()); }

glm::vec3 endOf(const Skeleton& s, size_t index) { return glm::vec3(s.segments.at(index).model[3]); }

// Unit direction the segment points to (the image of +Y under the frame).
glm::vec3 directionOf(const Skeleton& s, size_t index) { return glm::normalize(glm::vec3(s.segments.at(index).model[1])); }

std::string str(const glm::vec3& v) {
	return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
}

bool near(const glm::vec3& a, const glm::vec3& b, float eps = kEps) {
	return std::fabs(a.x - b.x) <= eps && std::fabs(a.y - b.y) <= eps && std::fabs(a.z - b.z) <= eps;
}

bool near(float a, float b, float eps = kEps) { return std::fabs(a - b) <= eps; }

#define CHECK_VEC(actual, expected) CHECK_MESSAGE(near((actual), (expected)), "got " << str(actual) << " expected " << str(expected))

bool finite(const glm::mat4& m) {
	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			if (!std::isfinite(m[c][r]))
				return false;
	return true;
}

}

TEST_CASE("Turtle: empty input gives only the root") {
	Skeleton s = run("");
	REQUIRE(s.segments.size() == 1);
	const treegen::SkeletonSegment& root = s.segments[0];
	CHECK(root.parent == -1);
	CHECK(root.childCount == 0);
	CHECK(treegen::isTip(root));
	CHECK(near(root.radius, 1.0f));
	CHECK(near(root.roll, 0.0f));
	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			CHECK(root.model[c][r] == (c == r ? 1.0f : 0.0f));
}

TEST_CASE("Turtle: the root takes initialRadius and an identity frame") {
	TreeParams p = makeParams();
	p.initialRadius = 2.5f;
	Skeleton s = run("F(1)", p);
	CHECK(near(s.segments[0].radius, 2.5f));
	CHECK(near(s.segments[1].radius, 2.5f));
}

TEST_CASE("Turtle: F(1) is one segment ending at (0,1,0)") {
	Skeleton s = run("F(1)");
	REQUIRE(s.segments.size() == 2);
	CHECK(s.segments[1].parent == 0);
	CHECK(s.segments[0].childCount == 1);
	CHECK(s.segments[1].childCount == 0);
	CHECK_VEC(endOf(s, 1), glm::vec3(0, 1, 0));
	CHECK(near(s.segments[1].radius, 1.0f));
	CHECK(near(s.segments[1].roll, 0.0f));
}

TEST_CASE("Turtle: F(1)F(1) is a chain ending at (0,2,0)") {
	Skeleton s = run("F(1)F(1)");
	REQUIRE(s.segments.size() == 3);
	CHECK(s.segments[1].parent == 0);
	CHECK(s.segments[2].parent == 1);
	CHECK(s.segments[1].childCount == 1);
	CHECK(s.segments[2].childCount == 0);
	CHECK_VEC(endOf(s, 1), glm::vec3(0, 1, 0));
	CHECK_VEC(endOf(s, 2), glm::vec3(0, 2, 0));
}

TEST_CASE("Turtle: f draws like F") {
	Skeleton s = run("f(2)F(3)");
	REQUIRE(s.segments.size() == 3);
	CHECK_VEC(endOf(s, 2), glm::vec3(0, 5, 0));
}

TEST_CASE("Turtle: branches give parent indices and child counts") {
	// 0 root, 1 trunk, 2 side branch, 3 trunk continuation
	Skeleton s = run("F(1)[^(90)F(1)]F(1)");
	REQUIRE(s.segments.size() == 4);
	CHECK(s.segments[1].parent == 0);
	CHECK(s.segments[2].parent == 1);
	CHECK(s.segments[3].parent == 1);
	CHECK(s.segments[0].childCount == 1);
	CHECK(s.segments[1].childCount == 2);
	CHECK(s.segments[2].childCount == 0);
	CHECK(s.segments[3].childCount == 0);
	CHECK_VEC(endOf(s, 2), glm::vec3(0, 1, 1));
	CHECK_VEC(endOf(s, 3), glm::vec3(0, 2, 0));
	CHECK_FALSE(treegen::isTip(s.segments[1]));
	CHECK(treegen::isTip(s.segments[2]));
	CHECK(treegen::isTip(s.segments[3]));
}

TEST_CASE("Turtle: parents always precede their children and child counts add up") {
	Skeleton s = run("F(1)[^(30)F(1)[<(30)F(1)]F(1)]F(1)[v(30)F(1)]F(1)");
	int children = 0;
	for (size_t i = 1; i < s.segments.size(); ++i) {
		CHECK(s.segments[i].parent >= 0);
		CHECK(s.segments[i].parent < static_cast<int>(i));
	}
	for (const treegen::SkeletonSegment& segment : s.segments)
		children += segment.childCount;
	CHECK(children == static_cast<int>(s.segments.size()) - 1);
}

TEST_CASE("Turtle: brackets restore position, rotation, radius and roll") {
	Skeleton s = run("F(1)[^(90)=(0.5)+(30)F(1)]F(1)");
	REQUIRE(s.segments.size() == 4);
	CHECK_VEC(endOf(s, 3), glm::vec3(0, 2, 0));
	CHECK(near(s.segments[3].radius, 1.0f));
	CHECK(near(s.segments[3].roll, 0.0f));
	CHECK(near(s.segments[2].radius, 0.5f));
}

TEST_CASE("Turtle: pitch ^ turns +Y towards +Z and v the other way") {
	CHECK_VEC(endOf(run("^(90)F(1)"), 1), glm::vec3(0, 0, 1));
	CHECK_VEC(endOf(run("v(90)F(1)"), 1), glm::vec3(0, 0, -1));
	Skeleton half = run("^(45)F(1)");
	CHECK_VEC(endOf(half, 1), glm::vec3(0, std::cos(kPi / 4), std::sin(kPi / 4)));
}

TEST_CASE("Turtle: yaw < turns +Y towards -X and > the other way") {
	CHECK_VEC(endOf(run("<(90)F(1)"), 1), glm::vec3(-1, 0, 0));
	CHECK_VEC(endOf(run(">(90)F(1)"), 1), glm::vec3(1, 0, 0));
}

TEST_CASE("Turtle: roll + and - keep the direction, turn the frame about Y and accumulate roll") {
	Skeleton plus = run("+(90)F(1)");
	CHECK_VEC(endOf(plus, 1), glm::vec3(0, 1, 0));
	CHECK(near(plus.segments[1].roll, kPi / 2));
	CHECK_VEC(glm::vec3(plus.segments[1].model[0]), glm::vec3(0, 0, -1));   // image of +X under R_y(+90)

	Skeleton minus = run("-(90)F(1)");
	CHECK(near(minus.segments[1].roll, -kPi / 2));
	CHECK_VEC(glm::vec3(minus.segments[1].model[0]), glm::vec3(0, 0, 1));

	Skeleton sum = run("+(30)+(30)-(10)F(1)");
	CHECK(near(sum.segments[1].roll, 50.0f * kPi / 180.0f));
	// the roll survives the segment boundary
	Skeleton carried = run("+(30)F(1)F(1)");
	CHECK(near(carried.segments[2].roll, 30.0f * kPi / 180.0f));
}

TEST_CASE("Turtle: rotations accumulate in order and carry over to the next segment") {
	// rotations are local (post-multiplied): after the pitch the turtle's own Z axis is world -Y, so the yaw turns
	// the +Z heading about -Y, i.e. towards -X
	Skeleton s = run("^(90)<(90)F(1)");
	CHECK_VEC(endOf(s, 1), glm::vec3(-1, 0, 0));
	Skeleton carry = run("^(90)F(1)F(1)");
	CHECK_VEC(endOf(carry, 2), glm::vec3(0, 0, 2));
}

TEST_CASE("Turtle: a polygon of 360 one-degree pitches closes") {
	std::string text;
	for (int i = 0; i < 360; ++i)
		text += "^(1)F(1)";
	Skeleton s = run(text);
	REQUIRE(s.segments.size() == 361);
	CHECK_MESSAGE(near(endOf(s, 360), glm::vec3(0, 0, 0), 2e-3f), "end " << str(endOf(s, 360)));
}

TEST_CASE("Turtle: defaults come from the parameters") {
	TreeParams p = makeParams();
	p.initialLength = 2.0f;
	p.initialRadius = 3.0f;
	p.angle = 30.0f * kPi / 180.0f;
	CHECK_VEC(endOf(run("F", p), 1), glm::vec3(0, 2, 0));
	CHECK(near(run("F", p).segments[1].radius, 3.0f));
	CHECK_VEC(endOf(run("^F", p), 1), glm::vec3(0, 2 * std::cos(kPi / 6), 2 * std::sin(kPi / 6)));
	CHECK(near(run("+F", p).segments[1].roll, kPi / 6));
	CHECK_VEC(endOf(run("<F", p), 1), glm::vec3(-2 * std::sin(kPi / 6), 2 * std::cos(kPi / 6), 0));
}

TEST_CASE("Turtle: length resets to initialLength after each F") {
	TreeParams p = makeParams();
	p.initialLength = 2.0f;
	Skeleton s = run("F(5)F", p);
	CHECK_VEC(endOf(s, 1), glm::vec3(0, 5, 0));
	CHECK_VEC(endOf(s, 2), glm::vec3(0, 7, 0));
}

TEST_CASE("Turtle: semicolon scales the angle used by later default rotations") {
	CHECK_VEC(endOf(run(";(0.5)^F(1)"), 1), glm::vec3(0, std::cos(kPi / 4), std::sin(kPi / 4)));
	// default factor 0.625, and it persists across segments and compounds
	CHECK_VEC(endOf(run(";^F(1)"), 1), glm::vec3(0, std::cos(kPi / 2 * 0.625f), std::sin(kPi / 2 * 0.625f)));
	Skeleton twice = run(";(0.5);(0.5)^F(1)");
	CHECK_VEC(endOf(twice, 1), glm::vec3(0, std::cos(kPi / 8), std::sin(kPi / 8)));
	Skeleton persists = run(";(0.5)F(1)^F(1)");
	CHECK_VEC(endOf(persists, 2), glm::vec3(0, 1 + std::cos(kPi / 4), std::sin(kPi / 4)));
}

TEST_CASE("Turtle: backtick scales length and radius and both persist") {
	Skeleton s = run("`(0.5)F(2)F(2)");
	CHECK_VEC(endOf(s, 1), glm::vec3(0, 1, 0));
	CHECK_VEC(endOf(s, 2), glm::vec3(0, 2, 0));
	CHECK(near(s.segments[1].radius, 0.5f));
	CHECK(near(s.segments[2].radius, 0.5f));
	Skeleton dflt = run("`F(1)");
	CHECK_VEC(endOf(dflt, 1), glm::vec3(0, 0.625f, 0));
	CHECK(near(dflt.segments[1].radius, 0.625f));
}

TEST_CASE("Turtle: equals scales the radius only and compounds") {
	Skeleton s = run("=(0.5)F(1)=(0.5)F(1)");
	CHECK_VEC(endOf(s, 2), glm::vec3(0, 2, 0));
	CHECK(near(s.segments[1].radius, 0.5f));
	CHECK(near(s.segments[2].radius, 0.25f));
	CHECK(near(run("=F(1)").segments[1].radius, 0.625f));
	CHECK(near(s.segments[0].radius, 1.0f));
}

TEST_CASE("Turtle: dollar is deterministic, finite, and also fine on a freshly reset turtle") {
	// a fresh turtle points straight up, parallel to the reference direction: the legacy code divided by zero here
	Skeleton fresh = run("$F(1)");
	REQUIRE(fresh.segments.size() == 2);
	CHECK(finite(fresh.segments[1].model));
	CHECK_VEC(endOf(fresh, 1), glm::vec3(0, 1, 0));

	Skeleton tilted = run("^(45)$F(1)");
	CHECK(finite(tilted.segments[1].model));
	Skeleton again = run("^(45)$F(1)");
	CHECK(tilted.segments[1].model == again.segments[1].model);
	// parameters of $ are not consumed as a rotation
	Skeleton withParam = run("$(30)F(1)");
	CHECK(withParam.segments.size() == 2);
	CHECK(finite(withParam.segments[1].model));
}

TEST_CASE("Turtle: T bends towards the tropism, the angle to it decreases monotonically") {
	TreeParams p = makeParams();
	p.tropismBendingFactor = 0.3f;
	std::string text = "^(60)F(1)";
	for (int i = 0; i < 6; ++i)
		text += "TF(1)";
	Skeleton s = run(text, p);
	REQUIRE(s.segments.size() == 8);
	float previous = 10.0f;
	for (size_t i = 1; i < s.segments.size(); ++i) {
		float angle = std::acos(glm::clamp(glm::dot(directionOf(s, i), p.tropism), -1.0f, 1.0f));
		CHECK_MESSAGE(angle < previous, "segment " << i << " angle " << angle << " previous " << previous);
		previous = angle;
	}
	// the first segment is 120 degrees away from straight down
	CHECK(near(std::acos(glm::dot(directionOf(s, 1), p.tropism)), 2 * kPi / 3, 1e-3f));
}

TEST_CASE("Turtle: T without an angle to bend (pointing along the tropism) changes nothing") {
	TreeParams p = makeParams();
	p.tropism = glm::vec3(0, 1, 0);
	CHECK_VEC(endOf(run("TF(1)", p), 1), glm::vec3(0, 1, 0));
}

TEST_CASE("Turtle: T parameter overrides the bending factor and zero bends nothing") {
	TreeParams p = makeParams();
	Skeleton none = run("^(60)F(1)T(0)F(1)", p);
	CHECK_VEC(directionOf(none, 2), directionOf(none, 1));
	Skeleton weak = run("^(60)F(1)T(0.1)F(1)", p);
	Skeleton strong = run("^(60)F(1)T(0.4)F(1)", p);
	float aWeak = std::acos(glm::dot(directionOf(weak, 2), p.tropism));
	float aStrong = std::acos(glm::dot(directionOf(strong, 2), p.tropism));
	CHECK(aStrong < aWeak);
}

TEST_CASE("Turtle: the identity start keeps rotations that precede the first F") {
	// glm::quat() is the zero quaternion in the pinned glm, the legacy interpreter lost these rotations (D-010)
	CHECK_VEC(endOf(run("^(90)F(1)"), 1), glm::vec3(0, 0, 1));
	CHECK_VEC(endOf(run("^(90)^(90)F(1)"), 1), glm::vec3(0, -1, 0));
}

TEST_CASE("Turtle: tilde draws uniform01 times the bound from the turtle stream, deterministic per seed") {
	treegen::Rng stream = treegen::Rng(5).derive(2);
	float first = stream.uniform01() * 2.0f;
	float second = stream.uniform01() * 2.0f;
	Skeleton s = run("F~(2)F~(2)", makeParams(), 5);
	REQUIRE(s.segments.size() == 3);
	CHECK(near(endOf(s, 1).y, first));
	CHECK(near(endOf(s, 2).y, first + second));
	CHECK(first >= 0.0f);
	CHECK(first < 2.0f);

	Skeleton same = run("F~(2)F~(2)", makeParams(), 5);
	CHECK(s.segments[2].model == same.segments[2].model);
	Skeleton other = run("F~(2)F~(2)", makeParams(), 6);
	CHECK(s.segments[1].model != other.segments[1].model);
}

TEST_CASE("Turtle: tilde works on rotations and ignores a missing bound") {
	treegen::Rng stream = treegen::Rng(11).derive(2);
	float draw = stream.uniform01() * 90.0f;
	Skeleton s = run("^~(90)F(1)", makeParams(), 11);
	float radians = draw * kPi / 180.0f;
	CHECK_VEC(endOf(s, 1), glm::vec3(0, std::cos(radians), std::sin(radians)));
	// no parameter: the default applies and no number is drawn
	Skeleton bare = run("^~F~(2)", makeParams(), 11);
	float expected = treegen::Rng(11).derive(2).uniform01() * 2.0f;
	CHECK(near(glm::length(endOf(bare, 1)), expected, 1e-3f));
}

TEST_CASE("Turtle: unknown symbols and their parameters are ignored") {
	Skeleton s = run("A(1,2)Z(-1)B(45)F(1)X~(3)");
	REQUIRE(s.segments.size() == 2);
	CHECK_VEC(endOf(s, 1), glm::vec3(0, 1, 0));
	CHECK(near(s.segments[1].roll, 0.0f));
	Skeleton noise = run("a b c 1 2 3 # F(1)");
	REQUIRE(noise.segments.size() == 2);
	CHECK_VEC(endOf(noise, 1), glm::vec3(0, 1, 0));
}

TEST_CASE("Turtle: Review Focus 6: a literal -1 parameter is honored") {
	// F(-1) is a segment of length -1, not 'no parameter'
	Skeleton length = run("F(-1)");
	REQUIRE(length.segments.size() == 2);
	CHECK_VEC(endOf(length, 1), glm::vec3(0, -1, 0));

	// +(-1): a roll of -1 degree, not the 90 degree default
	Skeleton roll = run("+(-1)F(1)");
	CHECK(near(roll.segments[1].roll, -kPi / 180.0f));
	// ^(-1): a pitch of -1 degree
	Skeleton pitch = run("^(-1)F(1)");
	CHECK_VEC(endOf(pitch, 1), glm::vec3(0, std::cos(kPi / 180.0f), -std::sin(kPi / 180.0f)));
	// ;(-1) flips the sign of the angle, =(-1) of the radius, `(-1) of length and radius
	CHECK_VEC(endOf(run(";(-1)^F(1)"), 1), glm::vec3(0, 0, -1));
	CHECK(near(run("=(-1)F(1)").segments[1].radius, -1.0f));
	Skeleton scaled = run("`(-1)F(2)");
	CHECK_VEC(endOf(scaled, 1), glm::vec3(0, -2, 0));
	CHECK(near(scaled.segments[1].radius, -1.0f));
	// T(-1) bends away from the tropism instead of using the factor 0.2
	TreeParams p = makeParams();
	Skeleton away = run("^(60)F(1)T(-1)F(1)", p);
	Skeleton dflt = run("^(60)F(1)TF(1)", p);
	CHECK(std::acos(glm::dot(directionOf(away, 2), p.tropism)) > std::acos(glm::dot(directionOf(dflt, 2), p.tropism)));
}

namespace {

std::string messageOf(const std::string& text) {
	try {
		run(text);
	} catch (const treegen::InterpretError& e) {
		return e.what();
	}
	return {};
}

}

TEST_CASE("Turtle: Review Focus 1: a closing bracket without an opening one throws InterpretError naming the module") {
	CHECK_THROWS_AS(run("]"), treegen::InterpretError);
	CHECK_THROWS_AS(run("F(1)]F(1)"), treegen::InterpretError);
	CHECK_THROWS_AS(run("[F(1)]]"), treegen::InterpretError);
	CHECK_THROWS_AS(run("[[F(1)]F(1)]]F(1)"), treegen::InterpretError);

	CHECK(messageOf("]").find("module 0") != std::string::npos);
	CHECK(messageOf("F(1)]F(1)").find("module 1") != std::string::npos);
	// modules: [ F ] ] F -> the second ']' is module 3
	CHECK(messageOf("[F(1)]]F(1)").find("module 3") != std::string::npos);
	CHECK(messageOf("F(1)]").find(']') != std::string::npos);
	// derived from treegen::Error as the contract says
	CHECK_THROWS_AS(run("]"), treegen::Error);
}

TEST_CASE("Turtle: an unmatched opening bracket at the end is allowed") {
	Skeleton s = run("F(1)[F(1)");
	REQUIRE(s.segments.size() == 3);
	CHECK(s.segments[2].parent == 1);
	Skeleton nested = run("[[F(1)[F(1)");
	CHECK(nested.segments.size() == 3);
}

TEST_CASE("Turtle: a 100000 segment chain") {
	treegen::ModuleString modules;
	modules.reserve(100000);
	for (int i = 0; i < 100000; ++i) {
		treegen::Module m;
		m.symbol = 'F';
		m.params.push_back(1.0f);
		modules.push_back(m);
	}
	auto start = std::chrono::steady_clock::now();
	Skeleton s = treegen::interpret(makeParams(), modules);
	double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
	std::cout << "[turtle] 100000 segments interpreted in " << ms << " ms" << std::endl;
	REQUIRE(s.segments.size() == 100001);
	CHECK_VEC(endOf(s, 100000), glm::vec3(0, 100000, 0));
	CHECK(s.segments[100000].parent == 99999);
	CHECK(s.segments[0].childCount == 1);
#ifdef NDEBUG
	CHECK(ms < 1000.0);
#endif
}

TEST_CASE("Turtle: a long wiggling chain keeps its frames orthonormal") {
	treegen::ModuleString modules;
	for (int i = 0; i < 20000; ++i) {
		const char symbols[] = {'^', '<', '+', 'F'};
		for (char symbol : symbols) {
			treegen::Module m;
			m.symbol = symbol;
			m.params.push_back(symbol == 'F' ? 0.01f : 7.0f);
			modules.push_back(m);
		}
	}
	Skeleton s = treegen::interpret(makeParams(), modules);
	REQUIRE(s.segments.size() == 20001);
	const glm::mat4& m = s.segments.back().model;
	CHECK(finite(m));
	CHECK_MESSAGE(near(glm::length(glm::vec3(m[0])), 1.0f, 2e-3f), "x axis length " << glm::length(glm::vec3(m[0])));
	CHECK_MESSAGE(near(glm::length(glm::vec3(m[1])), 1.0f, 2e-3f), "y axis length " << glm::length(glm::vec3(m[1])));
	CHECK_MESSAGE(near(glm::dot(glm::vec3(m[0]), glm::vec3(m[1])), 0.0f, 2e-3f), "x.y " << glm::dot(glm::vec3(m[0]), glm::vec3(m[1])));
}
