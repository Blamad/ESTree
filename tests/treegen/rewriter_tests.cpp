#include <doctest/doctest.h>

#include <cmath>
#include <set>
#include <string>
#include <vector>

#include <treegen/Error.h>
#include <treegen/Module.h>
#include <treegen/Rewriter.h>
#include <treegen/TreeParams.h>

using treegen::RewriteError;
using treegen::TreeParams;

namespace {

TreeParams makeParams(const std::string& axiom, int depth) {
	TreeParams params;
	params.axiom = axiom;
	params.depth = depth;
	return params;
}

void addRule(TreeParams& params, char symbol, const std::string& production, std::vector<std::string> names = {},
	float probability = 1.0f, int initialDepth = 0) {
	treegen::Rule rule;
	rule.symbol = symbol;
	rule.paramNames = std::move(names);
	rule.production = production;
	rule.probability = probability;
	rule.initialDepth = initialDepth;
	params.rules[symbol].push_back(rule);
}

std::string run(const TreeParams& params, uint32_t seed = 1) {
	treegen::RewriteOptions options;
	options.seed = seed;
	return treegen::toString(treegen::rewrite(params, options));
}

// Runs rewrite() and returns the RewriteError message ("" when nothing was thrown).
std::string errorOf(const TreeParams& params) {
	try {
		treegen::rewrite(params);
	}
	catch (const RewriteError& e) {
		return e.what();
	}
	return "";
}

bool contains(const std::string& text, const std::string& part) {
	return text.find(part) != std::string::npos;
}

}

TEST_CASE("rewrite: depth 0 returns the axiom") {
	TreeParams params = makeParams("F(2)A", 0);
	addRule(params, 'A', "FA");
	CHECK(run(params) == "F(2.000000)A");
}

TEST_CASE("rewrite: an empty axiom stays empty") {
	TreeParams params = makeParams("", 3);
	addRule(params, 'A', "FA");
	CHECK(run(params) == "");
}

TEST_CASE("rewrite: symbols without a rule pass through unchanged") {
	TreeParams params = makeParams("F(2)[+(45)A]B(1,2)", 2);
	addRule(params, 'A', "AA");
	CHECK(run(params) == "F(2.000000)[+(45.000000)AAAA]B(1.000000,2.000000)");
}

TEST_CASE("rewrite: parameters are bound and expressions evaluated") {
	TreeParams params = makeParams("A(2,3)", 1);
	addRule(params, 'A', "F(l*w)B(l+w,l-w)[C(l/w)]", { "l", "w" });
	CHECK(run(params) == "F(6.000000)B(5.000000,-1.000000)[C(0.666667)]");
}

TEST_CASE("rewrite: rule parameters accumulate over iterations") {
	TreeParams params = makeParams("F(1)", 3);
	addRule(params, 'F', "F(l*2)", { "l" });
	CHECK(run(params) == "F(8.000000)");
}

TEST_CASE("rewrite: chained operators follow precedence and associativity") {
	TreeParams params = makeParams("A", 1);
	addRule(params, 'A', "F(2+3*4)F((2+3)*4)F(-5*2)F(10/4)F(1-2-3)F(2*3*4)F(1+2*3)");
	CHECK(run(params) == "F(14.000000)F(20.000000)F(-10.000000)F(2.500000)F(-4.000000)F(24.000000)F(7.000000)");
}

TEST_CASE("rewrite: negative parameter values and a leading minus (legacy KNOWN BUG 4 and 5)") {
	TreeParams params = makeParams("A(-2)", 1);
	addRule(params, 'A', "F(l*3)F(-l)F(-5*2)F(3--l)", { "l" });
	CHECK(run(params) == "F(-6.000000)F(2.000000)F(-10.000000)F(1.000000)");
}

TEST_CASE("rewrite: adjacent identical parameters are all substituted (legacy KNOWN BUG 2)") {
	TreeParams params = makeParams("A(2)", 1);
	addRule(params, 'A', "F(x,x)G(x*x,x)", { "x" });
	CHECK(run(params) == "F(2.000000,2.000000)G(4.000000,2.000000)");
}

TEST_CASE("rewrite: a parameter name that is part of a longer name is a different identifier") {
	TreeParams params = makeParams("A(2)", 1);
	addRule(params, 'A', "F(l)", { "l" });
	CHECK(run(params) == "F(2.000000)");
	TreeParams other = makeParams("B(2)", 1);
	addRule(other, 'B', "F(ll)", { "l" });
	CHECK(contains(errorOf(other), "unknown identifier 'll'"));
}

TEST_CASE("rewrite: an empty production removes the symbol (legacy KNOWN BUG 7)") {
	TreeParams params = makeParams("A", 1);
	addRule(params, 'A', "");
	CHECK(run(params) == "");
	TreeParams several = makeParams("FAF", 2);
	addRule(several, 'A', "");
	CHECK(run(several) == "FF");
}

TEST_CASE("rewrite: symbols that are regex metacharacters can be rewritten (legacy KNOWN BUG 8)") {
	TreeParams params = makeParams("A", 2);
	addRule(params, '+', "-");
	addRule(params, 'A', "+B");
	addRule(params, '^', "v");
	addRule(params, '*', "x");
	CHECK(run(params) == "-B");
	TreeParams more = makeParams("^*[.]", 1);
	addRule(more, '^', "v");
	addRule(more, '*', "x");
	addRule(more, '.', "d");
	CHECK(run(more) == "vx[d]");
}

TEST_CASE("rewrite: initialDepth gates a rule by iteration index") {
	TreeParams params = makeParams("A", 1);
	addRule(params, 'A', "x", {}, 1.0f, 1);
	CHECK(run(params) == "A");
	params.depth = 2;
	CHECK(run(params) == "x");

	TreeParams two = makeParams("A", 3);
	addRule(two, 'A', "B", {}, 1.0f, 0);
	addRule(two, 'B', "C", {}, 1.0f, 2);
	CHECK(run(two) == "C");
}

TEST_CASE("rewrite: a draw past the cumulative probability keeps the module") {
	TreeParams params = makeParams("A", 3);
	addRule(params, 'A', "x", {}, 0.0f);
	CHECK(run(params, 1) == "A");
	CHECK(run(params, 2) == "A");
}

TEST_CASE("rewrite: probability 1 always rewrites") {
	TreeParams params = makeParams(std::string(2000, 'A'), 1);
	addRule(params, 'A', "x");
	CHECK(run(params) == std::string(2000, 'x'));
}

TEST_CASE("rewrite: selection frequencies over 10000 draws are within 3 sigma") {
	const int n = 10000;
	TreeParams params = makeParams(std::string(n, 'A'), 1);
	addRule(params, 'A', "x", {}, 0.3f);
	addRule(params, 'A', "y", {}, 0.5f);   // the remaining 0.2 keeps the module
	treegen::RewriteOptions options;
	options.seed = 12345;
	treegen::ModuleString result = treegen::rewrite(params, options);
	REQUIRE(result.size() == static_cast<size_t>(n));
	int x = 0, y = 0, a = 0;
	for (const treegen::Module& m : result) {
		if (m.symbol == 'x') ++x;
		else if (m.symbol == 'y') ++y;
		else if (m.symbol == 'A') ++a;
	}
	CHECK(x + y + a == n);
	auto within = [&](int count, double p) {
		double sigma = std::sqrt(n * p * (1.0 - p));
		CAPTURE(count);
		CAPTURE(p);
		return std::fabs(count - n * p) <= 3.0 * sigma;
	};
	CHECK(within(x, 0.3));
	CHECK(within(y, 0.5));
	CHECK(within(a, 0.2));
}

TEST_CASE("rewrite: the same seed gives identical output, other seeds differ") {
	TreeParams params = makeParams("A", 6);
	addRule(params, 'A', "F[+A][-A]", {}, 0.4f);
	addRule(params, 'A', "FA", {}, 0.4f);
	addRule(params, 'A', "FF", {}, 0.1f);

	std::string first = run(params, 42);
	CHECK(run(params, 42) == first);
	CHECK(run(params, 42) == first);

	std::set<std::string> distinct;
	for (uint32_t seed = 1; seed <= 10; ++seed)
		distinct.insert(run(params, seed));
	CHECK(distinct.size() > 1);
	CHECK(run(params, 43) != first);
}

TEST_CASE("rewrite: the options default to seed 0 and are deterministic too") {
	TreeParams params = makeParams("A", 5);
	addRule(params, 'A', "AB", {}, 0.5f);
	addRule(params, 'A', "BA", {}, 0.5f);
	CHECK(treegen::toString(treegen::rewrite(params)) == treegen::toString(treegen::rewrite(params)));
}

TEST_CASE("rewrite: rewriting does not modify the parameters") {
	TreeParams params = makeParams("A(1)", 3);
	addRule(params, 'A', "A(x+1)A(x)", { "x" });
	run(params);
	CHECK(params.axiom == "A(1)");
	CHECK(params.rules['A'].size() == 1);
	CHECK(params.rules['A'][0].production == "A(x+1)A(x)");
}

TEST_CASE("rewrite: a randomized module keeps its flag") {
	TreeParams params = makeParams("A", 1);
	addRule(params, 'A', "X~(5)F");
	treegen::ModuleString result = treegen::rewrite(params);
	REQUIRE(result.size() == 2);
	CHECK(result[0].randomized);
	CHECK(result[0].symbol == 'X');
	CHECK(result[0].params == std::vector<float>{ 5.0f });
	CHECK_FALSE(result[1].randomized);
}

TEST_CASE("Review Focus 3: exponential growth stops at maxModules with a RewriteError") {
	TreeParams params = makeParams("A", 14);
	addRule(params, 'A', "A[A]A");
	params.maxModules = 10000;
	std::string message = errorOf(params);
	CHECK_FALSE(message.empty());
	CHECK(contains(message, "maxModules = 10000"));
	CHECK_THROWS_AS(treegen::rewrite(params), RewriteError);

	// exactly at the limit is fine: 1 -> 3 -> 9 modules
	TreeParams exact = makeParams("A", 2);
	addRule(exact, 'A', "AAA");
	exact.maxModules = 9;
	CHECK(treegen::rewrite(exact).size() == 9);
	exact.maxModules = 8;
	CHECK_THROWS_AS(treegen::rewrite(exact), RewriteError);

	// the limit is checked for the axiom, too
	TreeParams axiom = makeParams("AAAA", 0);
	axiom.maxModules = 3;
	CHECK_THROWS_AS(treegen::rewrite(axiom), RewriteError);
}

TEST_CASE("Review Focus 3: the default growth limit is 5 000 000 modules") {
	CHECK(TreeParams().maxModules == 5000000);
}

TEST_CASE("Review Focus 4: a module with too few parameters for the rule is a RewriteError") {
	TreeParams params = makeParams("A(1)", 1);
	addRule(params, 'A', "F(l*w)", { "l", "w" });
	std::string message = errorOf(params);
	CHECK(contains(message, "arity"));
	CHECK(contains(message, "'A'"));
	CHECK(contains(message, "rule A(l,w)"));
	CHECK(contains(message, "F(l*w)"));
}

TEST_CASE("Review Focus 4: a module with too many parameters for the rule is a RewriteError") {
	TreeParams params = makeParams("A(1,2,3)", 1);
	addRule(params, 'A', "F(l)", { "l" });
	std::string message = errorOf(params);
	CHECK(contains(message, "arity"));
	CHECK(contains(message, "A(1.000000,2.000000,3.000000)"));
	CHECK(contains(message, "rule A(l)"));
}

TEST_CASE("Review Focus 4: a rule with parameters applied to a symbol without arguments (legacy KNOWN BUG 9)") {
	TreeParams params = makeParams("A", 1);
	addRule(params, 'A', "F(l*2)", { "l" });
	std::string message = errorOf(params);
	CHECK(contains(message, "arity"));
	CHECK(contains(message, "'A'"));
}

TEST_CASE("Review Focus 4: a parameterless rule applied to a module with arguments is an error") {
	TreeParams params = makeParams("A(1)", 1);
	addRule(params, 'A', "F");
	CHECK(contains(errorOf(params), "arity"));
}

TEST_CASE("Review Focus 4: the arity of an inapplicable rule is not checked") {
	TreeParams params = makeParams("A", 1);
	addRule(params, 'A', "F(l)", { "l" }, 1.0f, 5);   // not applicable at iteration 0
	CHECK(run(params) == "A");
}

TEST_CASE("Review Focus 5: 0/0 in an expression is a RewriteError naming the expression") {
	TreeParams params = makeParams("A", 1);
	addRule(params, 'A', "F(0/0)");
	std::string message = errorOf(params);
	CHECK(contains(message, "0/0"));
	CHECK(contains(message, "NaN"));
	CHECK(contains(message, "rule A"));
}

TEST_CASE("Review Focus 5: x/0 in an expression is a RewriteError naming the expression") {
	TreeParams params = makeParams("A(3)", 1);
	addRule(params, 'A', "F(x/0)", { "x" });
	std::string message = errorOf(params);
	CHECK(contains(message, "x/0"));
	CHECK(contains(message, "A(3.000000)"));
}

TEST_CASE("Review Focus 5: a value that overflows float is a RewriteError") {
	TreeParams params = makeParams("A(1e30)", 1);
	addRule(params, 'A', "F(x*x)", { "x" });
	CHECK(contains(errorOf(params), "x*x"));
}

TEST_CASE("Review Focus 5: a zero divisor produced in an earlier iteration is found, too") {
	TreeParams params = makeParams("A(1)", 2);
	addRule(params, 'A', "C(x)D(0)", { "x" });
	addRule(params, 'D', "E(1/y)", { "y" });
	CHECK(contains(errorOf(params), "1/y"));
}

TEST_CASE("rewrite: malformed productions are RewriteErrors naming the rule") {
	const char* bad[] = { "F(", "F)", "F(1,)", "F(1,,2)", "(1)", "F(1+)", "F(q)", "F~~(1)" };
	for (const char* production : bad) {
		CAPTURE(production);
		TreeParams params = makeParams("A", 1);
		addRule(params, 'A', production);
		std::string message = errorOf(params);
		CHECK(contains(message, "rule A"));
		CHECK(contains(message, production));
	}
}

TEST_CASE("rewrite: a malformed axiom is a RewriteError") {
	TreeParams params = makeParams("F(", 1);
	CHECK_THROWS_AS(treegen::rewrite(params), RewriteError);
}

TEST_CASE("rewrite: a broken rule that never becomes applicable is not reported") {
	TreeParams params = makeParams("A", 1);
	addRule(params, 'A', "F(", {}, 1.0f, 9);
	CHECK(run(params) == "A");
}
