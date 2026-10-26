#include <doctest/doctest.h>

#include <stdexcept>
#include <string>

#include <treegen/Error.h>
#include <treegen/Module.h>

using treegen::Module;
using treegen::ModuleString;

TEST_CASE("Module: parseModules reads symbols and parameters") {
	ModuleString m = treegen::parseModules("F(2)T+(45)A");
	REQUIRE(m.size() == 4);
	CHECK(m[0].symbol == 'F');
	REQUIRE(m[0].params.size() == 1);
	CHECK(m[0].params[0] == 2.0f);
	CHECK(m[1].symbol == 'T');
	CHECK(m[1].params.empty());
	CHECK(m[2].symbol == '+');
	REQUIRE(m[2].params.size() == 1);
	CHECK(m[2].params[0] == 45.0f);
	CHECK(m[3].symbol == 'A');
	CHECK(m[3].params.empty());
	for (const Module& module : m)
		CHECK_FALSE(module.randomized);
}

TEST_CASE("Module: toString is canonical (std::to_string(float), comma separated)") {
	CHECK(treegen::toString(treegen::parseModules("F(2)T+(45)A")) == "F(2.000000)T+(45.000000)A");
	CHECK(treegen::toString(treegen::parseModules("A(1.5,-2,0.25)")) == "A(1.500000,-2.000000,0.250000)");
}

TEST_CASE("Module: round trip is stable") {
	const std::string canonical = "F(2.000000)[+(45.000000)A(1.000000,2.500000)]T^(-12.500000)X~(5.000000)";
	CHECK(treegen::toString(treegen::parseModules(canonical)) == canonical);
	const std::string once = treegen::toString(treegen::parseModules("F(2)[A(.5)]"));
	CHECK(treegen::toString(treegen::parseModules(once)) == once);
}

TEST_CASE("Module: the empty text is the empty module string") {
	CHECK(treegen::parseModules("").empty());
	CHECK(treegen::toString({}) == "");
}

TEST_CASE("Module: brackets and punctuation symbols are modules without parameters") {
	ModuleString m = treegen::parseModules("[^v<>;`=$]");
	REQUIRE(m.size() == 10);
	CHECK(m.front().symbol == '[');
	CHECK(m.back().symbol == ']');
	CHECK(treegen::toString(m) == "[^v<>;`=$]");
}

TEST_CASE("Module: a tilde marks a randomized symbol") {
	ModuleString m = treegen::parseModules("X~(5)F");
	REQUIRE(m.size() == 2);
	CHECK(m[0].symbol == 'X');
	CHECK(m[0].randomized);
	REQUIRE(m[0].params.size() == 1);
	CHECK(m[0].params[0] == 5.0f);
	CHECK_FALSE(m[1].randomized);
}

TEST_CASE("Module: numbers accept a leading minus, a leading dot and an exponent") {
	ModuleString m = treegen::parseModules("F(-1)F(.25)F(1e2)");
	REQUIRE(m.size() == 3);
	CHECK(m[0].params[0] == -1.0f);
	CHECK(m[1].params[0] == 0.25f);
	CHECK(m[2].params[0] == 100.0f);
}

TEST_CASE("Module: whitespace between modules is ignored") {
	CHECK(treegen::toString(treegen::parseModules(" F(1) \t A ")) == "F(1.000000)A");
}

TEST_CASE("Module: malformed text throws RewriteError") {
	for (const char* text : { "F(", "F(1", "F(a)", "F(1,)", "F(,1)", "F()", "F(1 2)", ")", ",", "F(1))", "(1)", "~", "~(1)", "F~~(1)" }) {
		CAPTURE(text);
		CHECK_THROWS_AS(treegen::parseModules(text), treegen::RewriteError);
	}
}

TEST_CASE("Module: the parse error names the offset") {
	try {
		treegen::parseModules("F(1)A(x)");
		FAIL("expected RewriteError");
	}
	catch (const treegen::RewriteError& e) {
		CHECK(std::string(e.what()).find("offset 6") != std::string::npos);
	}
}

TEST_CASE("Error: the typed errors derive from treegen::Error and std::runtime_error") {
	CHECK_THROWS_AS(throw treegen::LoadError("x"), treegen::Error);
	CHECK_THROWS_AS(throw treegen::RewriteError("x"), treegen::Error);
	CHECK_THROWS_AS(throw treegen::InterpretError("x"), std::runtime_error);
}
