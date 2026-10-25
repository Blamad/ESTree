#include <treegen/Geometry.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <system_error>
#include <thread>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <treegen/Error.h>
#include <treegen/Rng.h>

namespace treegen {

namespace {

constexpr std::size_t kParallelVertexThreshold = 50000;

// Frame entries, radii and rolls beyond this are treated as garbage: the unnormalised normal is about
// radius * |entry| * 2 and its square must stay below FLT_MAX (1e8 -> ~1e33).
constexpr float kMaxMagnitude = 1e8f;

bool bounded(float value) {
	return std::fabs(value) <= kMaxMagnitude;   // false for NaN and Inf
}

// Finite, not absurdly large, and the x and z axes of the rotation part are independent, so no radial direction
// R * (cos, 0, sin) is the zero vector.
bool usableFrame(const glm::mat4& m) {
	for (int column = 0; column < 4; ++column)
		for (int row = 0; row < 4; ++row)
			if (!bounded(m[column][row]))
				return false;
	const glm::vec3 across = glm::cross(glm::vec3(m[0]), glm::vec3(m[2]));
	return glm::dot(across, across) > 1e-20f;
}

bool isFinite(const glm::mat4& m) {
	for (int column = 0; column < 4; ++column)
		for (int row = 0; row < 4; ++row)
			if (!std::isfinite(m[column][row]))
				return false;
	return true;
}

// sin/cos of the ring angles, already halved like the legacy code (`sinf(arg) / 2`).
struct RingTable {
	std::vector<float> halfSin, halfCos;
	float textureXStep;
};

RingTable makeRingTable(int ringDensity) {
	RingTable table;
	table.halfSin.resize(ringDensity);
	table.halfCos.resize(ringDensity);
	table.textureXStep = 1.0f / (ringDensity - 1);
	const float theta = 2 * glm::pi<float>() / float(ringDensity - 1);
	for (int j = 0; j < ringDensity; ++j) {
		const float arg = theta * j;
		table.halfSin[j] = std::sin(arg) / 2;
		table.halfCos[j] = std::cos(arg) / 2;
	}
	return table;
}

// The operation order is that of LindenmayerTreeMeshGenerator::computeRingPoint, so one platform gives the same bits.
void writeRing(const RingTable& table, int ringDensity, const glm::mat4& transform, float radius, float v, MeshVertex* out) {
	const glm::mat3 rotation(transform);
	for (int j = 0; j < ringDensity; ++j) {
		const float halfSin = table.halfSin[j];
		const float halfCos = table.halfCos[j];

		const glm::vec4 local(radius * halfCos, 0, radius * halfSin, 1);
		glm::vec3 normal = rotation * glm::vec3(local) * glm::vec3(2, 2, 2);
		const float lengthSquared = glm::dot(normal, normal);
		if (lengthSquared > 1e-30f)
			normal = glm::normalize(normal);
		else
			normal = glm::vec3(2 * halfCos, 0, 2 * halfSin);   // radius or frame scale underflowed: the local direction

		out[j].position = glm::vec3(transform * local);
		out[j].normal = normal;
		out[j].uv = glm::vec2(table.textureXStep * j, v);
	}
}

}

TreeGeometry buildGeometry(const Skeleton& skeleton, const TreeParams& params, const GeometryOptions& options) {
	if (options.ringDensity < 3)
		throw Error("buildGeometry: ringDensity must be at least 3, got " + std::to_string(options.ringDensity));

	const std::vector<SkeletonSegment>& segments = skeleton.segments;
	const std::size_t ringDensity = static_cast<std::size_t>(options.ringDensity);

	for (std::size_t i = 1; i < segments.size(); ++i)
		if (segments[i].parent < 0 || static_cast<std::size_t>(segments[i].parent) >= i)
			throw Error("buildGeometry: segment " + std::to_string(i) + " has parent " + std::to_string(segments[i].parent) +
				", expected an index below " + std::to_string(i));

	TreeGeometry result;
	GeometryStats& stats = result.stats;
	stats.segments = segments.empty() ? 0 : segments.size() - 1;

	// Pass 1: which segments get rings. A segment needs two usable rings: its own and its parent's.
	std::vector<unsigned char> sound(segments.size());
	for (std::size_t i = 0; i < segments.size(); ++i)
		sound[i] = usableFrame(segments[i].model) && bounded(segments[i].radius) && bounded(segments[i].roll);

	std::vector<uint32_t> emitted;
	emitted.reserve(stats.segments);
	for (std::size_t i = 1; i < segments.size(); ++i) {
		const SkeletonSegment& segment = segments[i];
		const SkeletonSegment& parent = segments[segment.parent];
		if (!sound[i] || !sound[segment.parent])
			++stats.skippedInvalidFrame;
		else if (segment.radius <= 0 || parent.radius <= 0)
			++stats.skippedZeroRadius;
		else
			emitted.push_back(static_cast<uint32_t>(i));   // segments.size() fits: vertex count is checked below
	}
	stats.emittedSegments = emitted.size();

	const std::size_t verticesPerSegment = 2 * ringDensity;
	const std::size_t indicesPerSegment = 6 * (ringDensity - 1);
	const std::size_t maxIndexValue = std::numeric_limits<uint32_t>::max();
	if (emitted.size() > maxIndexValue / verticesPerSegment)
		throw Error("buildGeometry: " + std::to_string(emitted.size()) + " segments with ringDensity " + std::to_string(ringDensity) +
			" need more than 2^32 - 1 vertices");

	// Pass 2: exact sizes, then every emitted segment fills its own slice of both arrays.
	MeshData& trunk = result.trunk;
	trunk.vertices.resize(emitted.size() * verticesPerSegment);
	trunk.indices.resize(emitted.size() * indicesPerSegment);

	if (!emitted.empty()) {
		const RingTable table = makeRingTable(options.ringDensity);
		MeshVertex* vertices = trunk.vertices.data();
		uint32_t* indices = trunk.indices.data();

		auto fill = [&](std::size_t begin, std::size_t end) {
			for (std::size_t k = begin; k < end; ++k) {
				const SkeletonSegment& segment = segments[emitted[k]];
				const SkeletonSegment& parent = segments[segment.parent];
				const std::size_t bottomOffset = k * verticesPerSegment;
				const std::size_t topOffset = bottomOffset + ringDensity;

				// The bottom ring is the parent's top ring turned to the new roll, as the legacy adapter did.
				const float roll = segment.roll - parent.roll;
				const glm::mat4 alignment = parent.model * glm::mat4_cast(glm::quat(glm::vec3(0, roll, 0)));
				writeRing(table, options.ringDensity, alignment, parent.radius, 0.0f, vertices + bottomOffset);
				writeRing(table, options.ringDensity, segment.model, segment.radius, 1.0f, vertices + topOffset);

				uint32_t* out = indices + k * indicesPerSegment;
				for (std::size_t i = 0; i + 1 < ringDensity; ++i) {
					*out++ = static_cast<uint32_t>(bottomOffset + i);
					*out++ = static_cast<uint32_t>(topOffset + i);
					*out++ = static_cast<uint32_t>(bottomOffset + i + 1);

					*out++ = static_cast<uint32_t>(topOffset + i);
					*out++ = static_cast<uint32_t>(topOffset + i + 1);
					*out++ = static_cast<uint32_t>(bottomOffset + i + 1);
				}
			}
		};

		std::size_t workers = options.threads;
		if (workers == 0) {
			const std::size_t cores = std::max(1u, std::thread::hardware_concurrency());
			workers = trunk.vertices.size() > kParallelVertexThreshold ? cores : 1;
		}
		workers = std::min(workers, emitted.size());

		auto chunkBegin = [&](std::size_t worker) { return emitted.size() * worker / workers; };

		std::vector<std::thread> pool;
		pool.reserve(workers - 1);
		struct Joiner {
			std::vector<std::thread>& threads;
			~Joiner() {
				for (std::thread& thread : threads)
					if (thread.joinable())
						thread.join();
			}
		} joiner{ pool };

		std::size_t next = 1;
		try {
			for (; next < workers; ++next)
				pool.emplace_back(fill, chunkBegin(next), chunkBegin(next + 1));
		} catch (const std::system_error&) {
			// no more threads available: the remaining chunks run below on this thread, the result is the same
		}
		fill(chunkBegin(0), chunkBegin(1));
		for (; next < workers; ++next)
			fill(chunkBegin(next), chunkBegin(next + 1));
	}

	// Leaves: one random draw per tip (a second one for the rotation), whether or not the leaf is later dropped.
	if (params.leafGrowthProb > 0.0f) {
		Rng rng = Rng(options.seed).derive(3);
		const glm::vec3 yAxis(0, 1, 0);
		for (const SkeletonSegment& segment : segments) {
			if (!isTip(segment))
				continue;
			if (rng.uniform01() > params.leafGrowthProb)
				continue;

			glm::mat4 model = segment.model;
			if (params.leavesAngleDiversity > 0) {
				const float angle = rng.uniform01() * 2 * params.leavesAngleDiversity - params.leavesAngleDiversity;
				model = model * glm::rotate(glm::mat4(1.0f), angle, yAxis);
			}
			if (isFinite(model))
				result.leafTransforms.push_back(model);
			else
				++stats.skippedLeaves;
		}
	}

	return result;
}

}
