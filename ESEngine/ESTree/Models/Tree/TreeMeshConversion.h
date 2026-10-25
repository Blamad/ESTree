#pragma once

// Pure conversions between the treegen output and the engine's mesh types. No OpenGL, no engine singletons:
// the header only needs glm, the plain Vertex / InstancedTransform structs and treegen's MeshData, so it is
// unit-tested headless (tests/adapter).
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <treegen/MeshData.h>

#include "Engine/Core/Vertex.h"
#include "Engine/GameObject/InstancedTransform.h"

namespace treeadapter {

// The engine vertex also carries a color the tree shaders do not read; it is zero, not indeterminate.
inline Vertex toEngineVertex(const treegen::MeshVertex &source) {
	Vertex vertex{};
	vertex.position = source.position;
	vertex.normal = source.normal;
	vertex.texCoords = source.uv;
	return vertex;
}

inline std::vector<Vertex> toEngineVertices(const std::vector<treegen::MeshVertex> &source) {
	std::vector<Vertex> vertices;
	vertices.reserve(source.size());
	for (const treegen::MeshVertex &vertex : source)
		vertices.push_back(toEngineVertex(vertex));
	return vertices;
}

// The engine's index type is int (GL gets them as GLuint, the values are never negative). Anything above
// INT_MAX would turn negative, so it is refused instead of wrapping.
inline int toEngineIndex(uint32_t index) {
	// (max) in parentheses: windows.h defines a max macro
	if (index > static_cast<uint32_t>((std::numeric_limits<int>::max)()))
		throw std::length_error("mesh index " + std::to_string(index) + " does not fit the engine's int index type");
	return static_cast<int>(index);
}

inline std::vector<int> toEngineIndices(const std::vector<uint32_t> &source) {
	std::vector<int> indices;
	indices.reserve(source.size());
	for (uint32_t index : source)
		indices.push_back(toEngineIndex(index));
	return indices;
}

// Mesh / InstancedMesh take their GL buffer capacities as int.
inline int checkedBufferSize(std::size_t count, const char *what) {
	if (count > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
		throw std::length_error(std::string(what) + " count " + std::to_string(count) + " does not fit the engine's int buffer size");
	return static_cast<int>(count);
}

// treegen leaf transforms are in tree space; the engine instances them in world space, so the tree's world
// matrix is premultiplied exactly as the old LindenmayerTree::generateInstancedLeaves did.
inline std::vector<InstancedTransform> toInstancedTransforms(const std::vector<glm::mat4> &leaves, const glm::mat4 &world) {
	std::vector<InstancedTransform> transforms;
	transforms.reserve(leaves.size());
	for (const glm::mat4 &leaf : leaves) {
		InstancedTransform transform;
		transform.modelMatrix = world * leaf;
		transforms.push_back(transform);
	}
	return transforms;
}

// 0 means "the caller decides" in treegen, so a fresh seed is never 0 (the logged seed must reproduce the tree).
inline uint32_t randomSeed() {
	std::random_device device;
	uint32_t seed = 0;
	while (seed == 0)
		seed = static_cast<uint32_t>(device());
	return seed;
}

// The seed rules of the console: the rule file's own non-zero seed wins, then the one requested with `seed <n>`
// (0 = none requested), then a fresh random one. `fresh` is injectable for tests.
inline uint32_t chooseSeed(uint32_t paramsSeed, uint32_t requestedSeed, const std::function<uint32_t()> &fresh = randomSeed) {
	if (paramsSeed != 0)
		return paramsSeed;
	if (requestedSeed != 0)
		return requestedSeed;
	const uint32_t seed = fresh();
	return seed != 0 ? seed : 1;
}

}
