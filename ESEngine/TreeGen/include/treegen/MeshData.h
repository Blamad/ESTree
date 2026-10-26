#pragma once

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

namespace treegen {

// One render-ready vertex. Eight floats, no padding: T07 copies it field by field into the engine's Vertex.
struct MeshVertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

static_assert(sizeof(MeshVertex) == 8 * sizeof(float), "MeshVertex must be tightly packed");

struct MeshData {
	std::vector<MeshVertex> vertices;
	std::vector<uint32_t>   indices;   // triangle list, indices.size() is a multiple of 3
};

}
