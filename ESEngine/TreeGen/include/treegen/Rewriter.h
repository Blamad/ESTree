#pragma once

#include <cstdint>

#include <treegen/Module.h>
#include <treegen/TreeParams.h>

namespace treegen {

struct RewriteOptions {
	// Seed of the rule-selection stream (Rng(seed).derive(1)). The same params and seed give the same result on
	// every platform. 0 is an ordinary seed here; callers that want varied trees pick a fresh one.
	uint32_t seed = 0;
};

// Rewrites params.axiom params.depth times.
//
// Per iteration (index i, starting at 0) every module whose symbol has rules is replaced; other modules pass
// through unchanged. The candidates are the rules of the symbol with initialDepth <= i, in declaration order.
// One number u in [0, 1) is drawn per rewritten module and rule k is chosen when
// sum(p[0..k-1]) <= u < sum(p[0..k]); a draw past the total keeps the module unchanged.
// The chosen rule must declare as many parameters as the module has. Its production is compiled once into
// literal modules whose parameters are expressions (+ - * /, unary minus, parentheses) over the rule's
// parameter names; they are evaluated per application (custom parameters of the rule file were already
// substituted by the loader).
//
// Throws RewriteError for: arity mismatch (message names symbol and rule), a malformed production or
// expression, a NaN or infinite parameter (0/0, x/0; message names the expression), and more than
// params.maxModules modules (before or after any iteration).
ModuleString rewrite(const TreeParams& params, const RewriteOptions& options = {});

}
