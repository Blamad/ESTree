#include <doctest/doctest.h>

#include <cmath>
#include <string>
#include <vector>

#include <treegen/Error.h>
#include <treegen/Expression.h>

using treegen::Expression;

namespace {

double eval(const std::string& text, const std::vector<std::string>& names = {}, const std::vector<float>& args = {}) {
	return Expression::parse(text, names).evaluate(args);
}

}

TEST_CASE("Expression: precedence and associativity table") {
	CHECK(eval("2+3*4") == 14.0);
	CHECK(eval("(2+3)*4") == 20.0);
	CHECK(eval("-5*2") == -10.0);
	CHECK(eval("10/4") == 2.5);
	CHECK(eval("1-2-3") == -4.0);
	CHECK(eval("8/4/2") == 1.0);
	CHECK(eval("2*3*4") == 24.0);
	CHECK(eval("1+2*3") == 7.0);
	CHECK(eval("2-3*4+5") == -5.0);
	CHECK(eval("2*(3+4)*5") == 70.0);
	CHECK(eval("((7))") == 7.0);
}

TEST_CASE("Expression: unary minus and plus") {
	CHECK(eval("-5") == -5.0);
	CHECK(eval("--5") == 5.0);
	CHECK(eval("2--3") == 5.0);
	CHECK(eval("2*-3") == -6.0);
	CHECK(eval("-(1+2)") == -3.0);
	CHECK(eval("+4") == 4.0);
	CHECK(eval("3-+2") == 1.0);
	CHECK(eval("-2*-2") == 4.0);
}

TEST_CASE("Expression: literals") {
	CHECK(eval("0.5") == 0.5);
	CHECK(eval(".25*4") == 1.0);
	CHECK(eval("1.5e1") == 15.0);
	CHECK(eval("  7 ") == 7.0);
	CHECK(eval(" 1 +\t2 * 3 ") == 7.0);
}

TEST_CASE("Expression: identifiers are bound to arguments by position") {
	CHECK(eval("l*lr", { "l", "lr" }, { 2.0f, 1.5f }) == 3.0);
	CHECK(eval("w+l", { "l", "w" }, { 1.0f, 10.0f }) == 11.0);
	CHECK(eval("a_1*2", { "a_1" }, { 4.0f }) == 8.0);
	CHECK(eval("-l*3", { "l" }, { -2.0f }) == 6.0);
	CHECK(eval("l", { "l" }, { 0.1f }) == doctest::Approx(0.1).epsilon(1e-6));
	Expression e = Expression::parse("l*l-1", { "l" });
	CHECK(e.evaluate(std::vector<float>{ 3.0f }) == 8.0);
	CHECK(e.evaluate(std::vector<float>{ 4.0f }) == 15.0);
}

TEST_CASE("Expression: isConstant and text") {
	CHECK(Expression::parse("2*3", {}).isConstant());
	CHECK_FALSE(Expression::parse("l*3", { "l" }).isConstant());
	CHECK(Expression::parse("l*3", { "l" }).text() == "l*3");
}

TEST_CASE("Expression: division by zero evaluates to Inf or NaN (the rewriter rejects them)") {
	CHECK(std::isnan(eval("0/0")));
	CHECK(std::isinf(eval("1/0")));
	CHECK(std::isinf(eval("x/0", { "x" }, { 5.0f })));
}

TEST_CASE("Expression: malformed text throws RewriteError naming the expression") {
	const char* bad[] = { "", "   ", "1+", "*2", "(1+2", "1+2)", "1 2", "2x", ".", "1..2", "a", "$", "1+*2", "()" };
	for (const char* text : bad) {
		CAPTURE(text);
		CHECK_THROWS_AS(Expression::parse(text, {}), treegen::RewriteError);
	}
	try {
		Expression::parse("l*q", { "l" });
		FAIL("expected a RewriteError");
	}
	catch (const treegen::RewriteError& e) {
		std::string message = e.what();
		CHECK(message.find("l*q") != std::string::npos);
		CHECK(message.find("'q'") != std::string::npos);
	}
}

TEST_CASE("Expression: nesting is bounded") {
	CHECK_THROWS_AS(Expression::parse(std::string(1000, '(') + "1" + std::string(1000, ')'), {}), treegen::RewriteError);
	CHECK_THROWS_AS(Expression::parse(std::string(1000, '-') + "1", {}), treegen::RewriteError);
	CHECK(eval(std::string(20, '(') + "1" + std::string(20, ')')) == 1.0);
}

TEST_CASE("Expression: evaluate rejects too few arguments") {
	Expression e = Expression::parse("l*w", { "l", "w" });
	CHECK_THROWS_AS(e.evaluate(std::vector<float>{ 1.0f }), treegen::RewriteError);
}
