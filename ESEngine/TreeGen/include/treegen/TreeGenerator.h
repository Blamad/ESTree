#pragma once

// The only header a consumer needs: it pulls in the whole pipeline (params, rewriter, turtle, geometry).
#include <cstdint>

#include <treegen/Error.h>
#include <treegen/Geometry.h>
#include <treegen/MeshData.h>
#include <treegen/Module.h>
#include <treegen/Rewriter.h>
#include <treegen/Rng.h>
#include <treegen/Skeleton.h>
#include <treegen/TreeParams.h>
#include <treegen/Turtle.h>

namespace treegen {

struct GenerateOptions {
	uint32_t seed = 0;        // 0 = use params.seed
	int      ringDensity = 9;
	unsigned threads = 0;     // see GeometryOptions::threads
};

struct TreeModel {
	Skeleton skeleton;
	TreeGeometry geometry;
	ModuleString modules;   // the rewritten production, kept for diagnostics and export
};

// rewrite -> interpret -> buildGeometry with one seed. Each stage derives its own random stream from it
// (1 = rewrite, 2 = turtle, 3 = leaves), so no stage depends on how many numbers another one drew.
// Throws RewriteError, InterpretError (and Error from buildGeometry for an invalid ringDensity).
TreeModel generateTree(const TreeParams& params, const GenerateOptions& options = {});

}
