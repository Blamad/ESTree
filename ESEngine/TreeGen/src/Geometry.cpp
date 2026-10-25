#include <treegen/Geometry.h>

#include <treegen/Error.h>

namespace treegen {

TreeGeometry buildGeometry(const Skeleton&, const TreeParams&, const GeometryOptions&) {
	throw Error("buildGeometry: not implemented");
}

}
