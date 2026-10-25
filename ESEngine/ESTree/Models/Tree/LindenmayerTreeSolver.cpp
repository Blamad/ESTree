#include "LindenmayerTreeSolver.h"

#include <random>

#include <treegen/Module.h>
#include <treegen/Rewriter.h>

std::string LindenmayerTreeSolver::generateTreeProduction(LindenmayerTreeParams &params) {
	treegen::RewriteOptions options;
	options.seed = params.seed != 0 ? params.seed : std::random_device{}();
	return treegen::toString(treegen::rewrite(params, options));
}
