#pragma once

#include <filesystem>
#include <vector>

#include <glm/glm.hpp>

#include <treegen/MeshData.h>

namespace treegen {

// Writes a Wavefront OBJ file (no MTL): object `trunk` with the trunk mesh, then object `leaves` with one
// double-sided 2x2 quad per leaf transform (the engine's leaf panel: corners (+-1, +-1, 0), front normal +Z, back
// normal -Z, 8 vertices and 4 triangles per leaf, vertices transformed by the matrix, normals by its normal matrix).
// An object without vertices is omitted. Faces are `f v/vt/vn` with 1-based indices that continue across objects.
// Numbers use '.' and fixed notation whatever the global locale is, lines end with '\n', the output is deterministic.
// Creates missing parent directories. Throws treegen::Error when the file cannot be created or written, or when a
// triangle index of `trunk` is out of range.
void writeObj(const std::filesystem::path& file, const MeshData& trunk, const std::vector<glm::mat4>& leaves);

}
