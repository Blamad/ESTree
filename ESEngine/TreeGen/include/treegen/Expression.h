#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace treegen {

// Numeric expression over a rule's formal parameters: literals, identifiers, + - * /, unary - and +, parentheses.
// Standard precedence (unary > * / > + -), left associative. Parsed once into a postfix program; evaluating
// does not allocate. evaluate() does not reject NaN/Inf (0/0, x/0): the rewriter does, because only it knows
// which rule and module the value belongs to.
class Expression {
public:
	// `names` are the identifiers an expression may use; the i-th name is bound to args[i] of evaluate().
	// Throws RewriteError (message names the expression text and the offset) on malformed text, an unknown
	// identifier or nesting deeper than maxDepth.
	static Expression parse(std::string_view text, const std::vector<std::string>& names);

	double evaluate(const float* args, std::size_t count) const;
	double evaluate(const std::vector<float>& args) const { return evaluate(args.data(), args.size()); }

	bool isConstant() const { return constant; }
	const std::string& text() const { return source; }

	static constexpr std::size_t maxDepth = 64;

private:
	enum class Op : unsigned char { Literal, Param, Negate, Add, Sub, Mul, Div };
	struct Instruction {
		Op op;
		std::size_t index;   // Param: argument index
		double value;        // Literal
	};

	std::string source;
	std::vector<Instruction> program;
	std::size_t paramCount = 0;
	bool constant = false;

	friend class ExpressionParser;
};

}
