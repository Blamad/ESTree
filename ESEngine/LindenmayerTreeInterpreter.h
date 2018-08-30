#ifndef LINDENMAYER_TREE_INTERPRETER_H
#define LINDENMAYER_TREE_INTERPRETER_H

#include <stack>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Segment.h"
#include "LindenmayerTreeParams.h"
#include "LindenmayerTreeMeshGenerator.h"

using namespace glm;

class LindenmayerTreeInterpreter {
public:
	static vector<shared_ptr<Segment>> generateMeshData(LindenmayerTreeParams &params, string product, LindenmayerTreeMeshGenerator *meshGenerator);

private:
	static boost::variate_generator<boost::mt19937, boost::uniform_real<>> randomGenerator;

	static shared_ptr<Segment> createRoot(SegmentTransform &transform);
	static float getNumericParameter(string product, int index);
	static float returnNewIndexAfterParameter(string product, int index);
	static quat applyTropism(SegmentTransform &transform, vec3 tropismDir, float bendingFactor);
	static quat restoreHorizontalOrientation(SegmentTransform &transform);
	static quat restoreHorizontalOrientationAlfa(SegmentTransform &transform);
	static quat getQuaternionTransformingFromTo(vec3 from, vec3 to);
	static shared_ptr<Segment> createSegment(shared_ptr<Segment> parent, SegmentTransform &transform, LindenmayerTreeMeshGenerator *meshGenerator);

	static vec3 getFrontVector(quat q) {
		//https://www.gamedev.net/forums/topic/56471-extracting-direction-vectors-from-quaternion/
		//Quat up vec (tutle's front)
		return vec3(2 * (q.x*q.y - q.w * q.z), 1 - 2 * (q.x*q.x + q.z * q.z), 2 * (q.y*q.z + q.w * q.x));
		//Quat front vec
		//return vec3(2 * (q.x*q.z + q.w * q.y), 2 * (q.y*q.z - q.w * q.x), 1 - 2 * (q.x*q.x + q.y * q.y));
	}

	static vec3 getProperFrontVector(quat q) {
		return vec3(2 * (q.x*q.z + q.w * q.y), 2 * (q.y*q.z - q.w * q.x), 1 - 2 * (q.x*q.x + q.y * q.y));
	}

	static vec3 getLeftVector(quat q) {
		return vec3(1 - 2 * (q.y*q.y + q.z*q.z), 2 * (q.x*q.y + q.w * q.z), 2 * (q.x*q.z - q.w * q.y));
	}

	static float toRadians(float angle) {
		return angle * 3.14159265359 / 180;
	}

	static float fromRadians(float radians) {
		return radians * 180 / 3.14159265359;
	}

	//Stuff
	static string number(float val) {
		if (val < 0.001 && val > -0.001)
			return "0";
		else
			return to_string(val);
	}

	static void printVec3(vec3 val) {
		cout << number(val.x) + " " + number(val.y) + " " + number(val.z) << endl;
	}

	static void printMat4(mat4 val) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				cout << number(val[j][i]) << " ";
			cout << endl;
		}
	}
};

#endif