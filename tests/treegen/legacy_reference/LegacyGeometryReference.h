// FROZEN REFERENCE - do not "improve" it, do not use it from production code. Deleted in T07.
//
// The trunk geometry of ESEngine/ESTree/Models/Tree/LindenmayerTreeMeshGenerator.cpp (enqueueGenerationData,
// createIndiciesForSegment, computeRingPoint) and of the segment replay in LindenmayerTreeInterpreter.cpp
// (bottom ring from the parent's model turned by `roll - parent.roll` about Y with v = 0, top ring from the
// segment's model with v = 1), copied so that treegen::buildGeometry can be compared with it without linking the
// legacy generator, which needs the OpenGL based Mesh class. Differences from the original, all mechanical:
//   - Mesh/Vertex are replaced by plain structs, the boost thread split by one sequential loop over the same
//     per-vertex function (the legacy chunks were independent, so the result is the same).
//   - Segments are plain structs with parent indices (as in LegacyTurtleReference.h) instead of shared_ptr<Segment>.
//   - Nothing is skipped or guarded: like the original this produces NaN for a zero radius or a singular frame.
#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4244 4267 4305 4456 4458)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wreorder"
#endif

namespace legacy_reference {

struct GeometrySegment {
	int parent = -1;
	glm::mat4 model = glm::mat4(1);
	float radius = 0;
	float roll = 0;
};

struct GeometryVertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct GeometryResult {
	std::vector<GeometryVertex> vertices;
	std::vector<int> indices;
};

namespace detail {

class VertexGenerationAttributes {
public:
	VertexGenerationAttributes(int verticesOffset, int vertexNumber, float &theta, float &radius, glm::mat4 &transform, int textureY)
		: verticesOffset(verticesOffset), vertexNumber(vertexNumber), theta(theta), radius(radius), transform(transform), textureY(textureY) { };

	int verticesOffset;
	int vertexNumber;
	float theta;
	float radius;
	glm::mat4 transform = glm::mat4(1);
	int textureY;
};

class MeshGenerator {
public:
	MeshGenerator(GeometryResult *mesh, int &ringDensity) : mesh(mesh), ringDensity(ringDensity) {
		this->textureXStep = 1.0f / (ringDensity - 1);
	};

	void enqueueGenerationData(float &radius, glm::mat4 &transform, int textureY) {
		float theta = 2 * glm::pi<float>() / float(ringDensity - 1);
		int verticesOffset = mesh->vertices.size();

		for (int j = 0; j < ringDensity; j++) {
			mesh->vertices.push_back(GeometryVertex());
			threadGenerationData.push_back(VertexGenerationAttributes(verticesOffset, j, theta, radius, transform, textureY));
		}
	}

	void createIndiciesForSegment() {
		int bottomOffset = getVerticesLength() - 2 * getVerticesPerRing();
		int topOffset = bottomOffset + getVerticesPerRing();
		for (int i = 0; i < getVerticesPerRing() - 1; i++) {
			mesh->indices.push_back(bottomOffset + i);
			mesh->indices.push_back(topOffset + i);
			mesh->indices.push_back(bottomOffset + i + 1);

			mesh->indices.push_back(topOffset + i);
			mesh->indices.push_back(topOffset + i + 1);
			mesh->indices.push_back(bottomOffset + i + 1);
		}
	}

	size_t getVerticesLength() { return mesh->vertices.size(); }
	int getVerticesPerRing() { return ringDensity; }

	void generateMesh() {
		computeRingPoint(0, (int)threadGenerationData.size(), this->mesh);
	}

private:
	std::vector<VertexGenerationAttributes> threadGenerationData;
	float textureXStep;
	int ringDensity;
	GeometryResult *mesh = nullptr;

	void computeRingPoint(int startIndex, int endIndex, GeometryResult *mesh) {
		float arg, sin, cos;
		glm::vec4 tmpPosition;
		glm::vec3 position, normal;
		GeometryVertex *vert;
		VertexGenerationAttributes *vga;

		for (int i = startIndex; i < endIndex; i++) {
			vga = &threadGenerationData[i];
			arg = vga->theta*vga->vertexNumber;
			sin = sinf(arg) / 2;
			cos = cosf(arg) / 2;

			glm::mat3 rotationMatrix(vga->transform);

			tmpPosition = glm::vec4(vga->radius * cos, 0, vga->radius * sin, 1);
			position = glm::vec3(vga->transform * tmpPosition);
			normal = normalize(rotationMatrix * glm::vec3(tmpPosition) * glm::vec3(2, 2, 2));

			//output verticies
			vert = &mesh->vertices[vga->verticesOffset + vga->vertexNumber];
			vert->position = position;
			vert->normal = normal;
			vert->texCoords = glm::vec2(textureXStep * vga->vertexNumber, vga->textureY);
		}
	}
};

}

// segments[0] is the root (no rings); every other segment's parent has a smaller index.
inline GeometryResult generateTrunk(const std::vector<GeometrySegment> &segments, int ringDensity) {
	GeometryResult result;
	detail::MeshGenerator meshGenerator(&result, ringDensity);

	std::vector<float> radii(segments.size());   // the legacy generator took the radius by reference
	for (size_t i = 0; i < segments.size(); i++) {
		radii[i] = segments[i].radius;
		if (i == 0)
			continue;

		const GeometrySegment &parent = segments[segments[i].parent];

		//Create textured bottom ring and rotate it to match top ring rotation:
		float roll = segments[i].roll - parent.roll;

		glm::mat4 alignmentTransform = parent.model * mat4_cast(glm::quat(glm::vec3(0, roll, 0)));
		meshGenerator.enqueueGenerationData(radii[segments[i].parent], alignmentTransform, 0);

		//Enqueue top ring creation:
		glm::mat4 model = segments[i].model;
		meshGenerator.enqueueGenerationData(radii[i], model, 1);

		//Link two rings
		meshGenerator.createIndiciesForSegment();
	}

	meshGenerator.generateMesh();
	return result;
}

}

#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
