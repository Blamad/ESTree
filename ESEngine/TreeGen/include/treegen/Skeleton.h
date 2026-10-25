#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace treegen {

struct SkeletonSegment {
	int       parent;                 // index into Skeleton::segments, -1 for the root
	glm::mat4 model;                  // frame at the segment's END (same as legacy Segment::modelMatrix)
	float     radius;                 // legacy "radius" semantics: a DIAMETER in geometry, see docs/architecture/overview.md
	float     roll;                   // accumulated roll about the segment's own axis, radians
	int       childCount = 0;
};

// Segments are stored in creation order, so a parent always has a smaller index than its children.
struct Skeleton {
	std::vector<SkeletonSegment> segments;   // segments[0] is the root: identity frame, zero length
};

inline bool isTip(const SkeletonSegment& s) { return s.childCount == 0; }

}
