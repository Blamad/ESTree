#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

#include <treegen/MeshData.h>
#include <treegen/Skeleton.h>
#include <treegen/TreeParams.h>

namespace treegen {

struct GeometryOptions {
	int      ringDensity = 9;   // vertices per ring including the duplicated seam vertex; must be >= 3
	uint32_t seed = 0;          // leaf selection and rotation, from Rng(seed).derive(3)
	// Worker threads for the vertex computation. 0 = automatic: hardware_concurrency() threads when the mesh has
	// more than 50 000 vertices, otherwise none. 1 = never spawn a thread. n > 1 = exactly min(n, segments)
	// threads, whatever the size. The result is bit-identical for every value.
	unsigned threads = 0;
};

// What buildGeometry skipped. `segments` counts the non-root skeleton segments.
struct GeometryStats {
	std::size_t segments = 0;
	std::size_t emittedSegments = 0;       // segments that produced rings and triangles
	std::size_t skippedZeroRadius = 0;     // own or parent radius <= 0
	std::size_t skippedInvalidFrame = 0;   // NaN/Inf/huge radius, matrix or roll, or a singular frame (own or parent's)
	std::size_t skippedLeaves = 0;         // kept leaves dropped because their transform is not finite
};

struct TreeGeometry {
	MeshData trunk;
	std::vector<glm::mat4> leafTransforms;   // one per kept tip, in the tree's local space (no world transform)
	GeometryStats stats;
};

// Builds the trunk like the legacy LindenmayerTreeMeshGenerator, without its fixed buffers:
//   - every non-root segment emits a bottom ring (the parent's frame rotated about Y by segment.roll - parent.roll,
//     the parent's radius, v = 0) and a top ring (the segment's frame and radius, v = 1), `ringDensity` vertices
//     each, then 2 * (ringDensity - 1) triangles joining them. The root emits nothing by itself.
//   - a ring vertex j has local position (radius*cos(t)/2, 0, radius*sin(t)/2) with t = 2*pi*j/(ringDensity-1),
//     transformed by the frame; **SkeletonSegment::radius is a legacy DIAMETER**, the geometric radius is half.
//     Vertex ringDensity-1 repeats vertex 0 geometrically and only differs in u (1 instead of 0): the UV seam.
//   - u = j / (ringDensity-1). The normal is radial (the frame's 3x3 applied to the local direction, normalised),
//     there is no smoothing across segments.
//   - segments with a radius <= 0 (own or the parent's) or with non-finite/singular frames are skipped: no
//     vertices, no indices, counted in TreeGeometry::stats. The mesh never contains NaN or Inf.
//   - the vertex and index counts are exact: 2*ringDensity and 6*(ringDensity-1) per emitted segment.
// Leaves: every tip (isTip, root included, as in the legacy loop) is kept with probability params.leafGrowthProb
// (u <= leafGrowthProb, u from Rng(options.seed).derive(3)); with params.leavesAngleDiversity > 0 the transform is
// `segment.model * rotation about Y by a uniform angle in [-d, d]`. leafGrowthProb <= 0 gives no leaves.
//
// Throws treegen::Error for ringDensity < 3, a segment whose parent index is not smaller than its own (or < 0 for
// a non-root segment) and a mesh that would need more than 2^32 - 1 vertices.
TreeGeometry buildGeometry(const Skeleton& skeleton, const TreeParams& params, const GeometryOptions& options = {});

}
