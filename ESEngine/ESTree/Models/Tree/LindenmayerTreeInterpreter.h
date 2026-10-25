#ifndef LINDENMAYER_TREE_INTERPRETER_H
#define LINDENMAYER_TREE_INTERPRETER_H

#include <memory>
#include <stack>
#include <string>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Segment.h"
#include "LindenmayerTreeParams.h"
#include "LindenmayerTreeMeshGenerator.h"

// Adapter over treegen::interpret (T05). The turtle itself lives in ESEngine/TreeGen; this class replays the resulting
// Skeleton into the mesh generator and builds the Segment tree the tree code still uses. Deleted in T07.
class LindenmayerTreeInterpreter {
public:
	static std::vector<std::shared_ptr<Segment>> generateMeshData(LindenmayerTreeParams &params, std::string product, LindenmayerTreeMeshGenerator *meshGenerator);
};

#endif
