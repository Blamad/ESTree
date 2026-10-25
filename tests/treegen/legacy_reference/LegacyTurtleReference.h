// FROZEN REFERENCE - do not "improve" it, do not use it from production code. Deleted in T07.
//
// This is the turtle math of ESEngine/ESTree/Models/Tree/LindenmayerTreeInterpreter.cpp as it was before T05
// (commit 5853916), copied so that treegen::interpret can be compared with it without linking the legacy class,
// which cannot be built without the OpenGL based mesh generator. Differences from the original, all mechanical:
//   - Segments are plain structs in a vector with parent indices instead of shared_ptr<Segment>; the mesh
//     generator calls of createSegment are dropped (only modelMatrix, radius and roll are kept).
//   - glm/gtx/vector_angle's angle(a, b) is written out (acos(clamp(dot(a, b), -1, 1))), which is its definition.
//   - The Boost random generator is replaced by the constant 0.5 (the harness feeds productions without '~').
//   - The root segment's roll is 0 (it was uninitialised memory).
//   - QuatInit selects how the two quaternions of the start transform are built. The legacy source wrote
//     `glm::quat()`; with the pinned glm (0.9.9.8, no GLM_FORCE_CTOR_INIT) that is the ZERO quaternion, which is
//     what the shipped binary ran with (AsCompiledLegacy): rotations before the first F are lost and the
//     accumulated rotation stays zero, so 'T' never bends anything. Identity is what the documentation and older
//     glm versions mean and what treegen implements (DECISIONS D-010).
//   - normalizeTotalRotation (default false = verbatim) renormalises the accumulated rotation after each F, the only
//     numerical difference of treegen::interpret: the legacy quaternions are never normalised and the tropism
//     quaternion has norm < 1, so the legacy accumulated rotation shrinks along a path with many 'T'.
// Everything else, including the unguarded `$` and the unnormalised tropism axis, is verbatim.
#pragma once

#include <cmath>
#include <cstdlib>
#include <stack>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

// The copied code predates the warning level of treegen; keep the copy verbatim and silence its warnings.
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4244 4267 4305 4456 4458)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

namespace legacy_reference {

struct Params {
	float initialLength = 1.0f;
	float initialRadius = 1.0f;
	float angle = 0.2617994f;   // radians
	float tropismBendingFactor = 0.2f;
	glm::vec3 tropism{0, -1, 0};
};

struct Segment {
	int parent = -1;
	glm::mat4 model = glm::mat4(1);
	float radius = 0;
	float roll = 0;
	int childCount = 0;
};

enum class QuatInit { Identity, AsCompiledLegacy };

namespace detail {

struct SegmentTransform {
	SegmentTransform(const glm::quat &rotation, const glm::quat &totalRotation, float length, float radius, float angle, float lengthScale = 1, float roll = 0) : rotation(rotation), totalRotation(totalRotation), length(length), radius(radius), lengthScale(lengthScale), roll(roll), angle(angle) {};
	float length;
	float lengthScale;
	float radius;
	float angle;

	glm::quat rotation = glm::quat(1.f, .0f, .0f, .0f);
	glm::quat totalRotation = glm::quat(1.f, .0f, .0f, .0f);
	float roll;
};

inline float toRadians(float angle) {
	return angle * 3.14159265359f / 180.f;
}

inline glm::vec3 getFrontVector(glm::quat q) {
	return glm::vec3(2 * (q.x*q.y - q.w * q.z), 1 - 2 * (q.x*q.x + q.z * q.z), 2 * (q.y*q.z + q.w * q.x));
}

inline float returnNewIndexAfterParameter(std::string product, int index) {
	int endIndex = index;
	while (product.at(endIndex) != ')')
		endIndex++;
	return endIndex;
}

inline float getNumericParameter(std::string product, int index) {
	float value = -1;
	bool randomizeValue = false;
	if (product.length() > (index + 1)) {
		if (product.at(index + 1) == '~') {
			randomizeValue = true;
			index++;
		}

		if (product.at(index + 1) == '(') {
			int offset = 0;
			while (product.length() > (index + offset) && product.at(index + offset) != ')')
				offset++;
			std::string strValue = product.substr(index + 2, offset - 2);
			value = atof(strValue.c_str());
		}
	}

	if (randomizeValue) {
		value = 0.5f * value;
	}

	return value;
}

inline glm::quat applyTropism(SegmentTransform &transform, glm::vec3 tropismDir, float bendingFactor) {
	glm::vec3 front = getFrontVector(transform.totalRotation * transform.rotation);
	glm::vec3 crossVec = glm::cross(front, tropismDir);
	float angleVal = std::acos(glm::clamp(glm::dot(front, tropismDir), -1.0f, 1.0f));
	angleVal *= sin(angleVal) * bendingFactor;

	glm::quat transformQuat = glm::angleAxis(-transform.roll, glm::vec3(0, 1, 0)) * glm::angleAxis(angleVal, crossVec) * glm::angleAxis(transform.roll, glm::vec3(0, 1, 0));

	return transform.rotation * transformQuat;
}

inline glm::quat restoreHorizontalOrientation(SegmentTransform &transform) {
	glm::vec3 worldUp(0, -1, 0);

	glm::vec3 front = getFrontVector(transform.rotation);
	glm::vec3 left = normalize(cross(front, worldUp));
	glm::vec3 up = normalize(cross(front, left));

	float matrix[16] = {
		left.x, front.x, up.x, 0,
		left.y, front.y, up.y, 0,
		left.z, front.z, up.z, 0,
		0,		0,		 0,	   0
	};

	glm::mat4 rotationMatrix = glm::make_mat4(matrix);
	return glm::quat(rotationMatrix);
}

inline int createSegment(std::vector<Segment> &segments, int parent, SegmentTransform &segTransform) {
	glm::mat4 segmentMatrix(1);
	if (segTransform.rotation.w != 1)
		segmentMatrix = segmentMatrix * mat4_cast(segTransform.rotation);
	segmentMatrix = translate(segmentMatrix, glm::vec3(0, segTransform.length, 0));

	Segment stem;
	stem.parent = parent;
	stem.model = segments[parent].model * segmentMatrix;
	stem.radius = segTransform.radius;
	stem.roll = segTransform.roll;

	segments[parent].childCount++;
	segments.push_back(stem);
	return static_cast<int>(segments.size()) - 1;
}

}

inline std::vector<Segment> interpret(const Params &params, std::string product, QuatInit init, bool normalizeTotalRotation = false) {
	using namespace detail;

	std::vector<Segment> segmentsVec;
	std::stack<int> segmentStack;
	std::stack<SegmentTransform> transformStack;

	int currentSegment;

	float customParameter = -1;

	const glm::quat startQuat = init == QuatInit::Identity ? glm::quat(1.f, .0f, .0f, .0f) : glm::quat();
	SegmentTransform transform = SegmentTransform(startQuat, startQuat, params.initialLength, params.initialRadius, params.angle);

	{
		Segment root;
		root.radius = transform.radius;
		segmentsVec.push_back(root);
		currentSegment = 0;
	}

	for (int i = 0; i < product.size(); i++) {
		char character = product.at(i);
		switch (character) {
		case '[':
			segmentStack.push(currentSegment);
			transformStack.push(SegmentTransform(transform.rotation, transform.totalRotation, transform.length, transform.radius, transform.angle, transform.lengthScale, transform.roll));
			break;
		case ']':
			currentSegment = segmentStack.top();
			segmentStack.pop();
			transform = transformStack.top();
			transformStack.pop();
			break;
		case '^': //pitch X axis +
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}
			transform.rotation *= glm::angleAxis(customParameter, glm::vec3(1, 0, 0));
			break;
		case 'v': //pitch X axis -
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}
			transform.rotation *= glm::angleAxis(-customParameter, glm::vec3(1, 0, 0));
			break;
		case '<': //yaw Z axis +
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= glm::angleAxis(customParameter, glm::vec3(0, 0, 1));
			break;
		case '>': //yaw Z axis -
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= glm::angleAxis(-customParameter, glm::vec3(0, 0, 1));
			break;
		case '+': //roll Y axis +
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.roll += customParameter;
			transform.rotation *= glm::angleAxis(customParameter, glm::vec3(0, 1, 0));
			break;
		case '-': //roll Y axis -
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.roll -= customParameter;
			transform.rotation *= glm::angleAxis(-customParameter, glm::vec3(0, 1, 0));
			break;
		case ';': //scale angle
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = 0.625;
			else
				i = returnNewIndexAfterParameter(product, i);

			transform.angle *= customParameter;
			break;
		case '`': //scale radius and length
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = 0.625;
			else
				i = returnNewIndexAfterParameter(product, i);

			transform.lengthScale *= customParameter;
			transform.radius *= customParameter;
			break;
		case '=': //scale thickness
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = 0.625;
			else
				i = returnNewIndexAfterParameter(product, i);
			transform.radius *= customParameter;
			break;
		case '$':
			transform.rotation = restoreHorizontalOrientation(transform);
			transform.roll = glm::roll(transform.rotation);
			break;
		case 'T':
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.tropismBendingFactor;
			else
				i = returnNewIndexAfterParameter(product, i);
			transform.rotation = applyTropism(transform, params.tropism, customParameter);
			break;
		case ' ':
		default:
			break;
		case 'f':
		case 'F':
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.length;
			else
				i = returnNewIndexAfterParameter(product, i);

			transform.length = customParameter * transform.lengthScale;
			currentSegment = createSegment(segmentsVec, currentSegment, transform);

			{
				glm::quat total = transform.totalRotation * transform.rotation;
				if (normalizeTotalRotation)
					total = glm::normalize(total);
				transform = SegmentTransform(glm::quat(1.f, .0f, .0f, .0f), total, params.initialLength, transform.radius, transform.angle, transform.lengthScale, transform.roll);
			}
			break;
		}
	}

	return segmentsVec;
}

}

#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
