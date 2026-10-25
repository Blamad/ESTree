#ifndef LINDENMAYER_TREE_SOLVER_H
#define LINDENMAYER_TREE_SOLVER_H

#include <string>

#include "LindenmayerTreeParams.h"

// Adapter over treegen::rewrite (the regex solver and its static RNG are gone). Deleted with the rest of the
// legacy pipeline in T07.
class LindenmayerTreeSolver {
public:
	// Throws treegen::RewriteError (malformed production, arity mismatch, NaN/Inf, growth limit).
	// Uses params.seed when it is non-zero, otherwise a fresh random seed (trees stay varied).
	static std::string generateTreeProduction(LindenmayerTreeParams &params);
};

#endif
