#include <treegen/TreeGenerator.h>

namespace treegen {

TreeModel generateTree(const TreeParams& params, const GenerateOptions& options) {
	const uint32_t seed = options.seed != 0 ? options.seed : params.seed;

	TreeModel model;

	RewriteOptions rewriteOptions;
	rewriteOptions.seed = seed;
	model.modules = rewrite(params, rewriteOptions);

	InterpretOptions interpretOptions;
	interpretOptions.seed = seed;
	model.skeleton = interpret(params, model.modules, interpretOptions);

	GeometryOptions geometryOptions;
	geometryOptions.ringDensity = options.ringDensity;
	geometryOptions.seed = seed;
	geometryOptions.threads = options.threads;
	model.geometry = buildGeometry(model.skeleton, params, geometryOptions);

	return model;
}

}
