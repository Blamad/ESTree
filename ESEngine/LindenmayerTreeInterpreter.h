#ifndef LINDENMAYER_TREE_INTERPRETER_H
#define LINDENMAYER_TREE_INTERPRETER_H

#include <stack>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Segment.h"
#include "LindenmayerTreeParams.h"
#include "LindenmayerTreeMeshGenerator.h"

class LindenmayerTreeInterpreter {
public:
	static std::vector<std::shared_ptr<Segment>> generateMeshData(LindenmayerTreeParams &params, std::string product, LindenmayerTreeMeshGenerator *meshGenerator);

private:
	static boost::variate_generator<boost::mt19937, boost::uniform_real<>> randomGenerator;

	static std::shared_ptr<Segment> createRoot(SegmentTransform &transform);
	static float getNumericParameter(std::string product, int index);
	static float returnNewIndexAfterParameter(std::string product, int index);
	static glm::quat applyTropism(SegmentTransform &transform, glm::vec3 tropismDir, float bendingFactor);
	static glm::quat restoreHorizontalOrientation(SegmentTransform &transform);
	static glm::quat restoreHorizontalOrientationAlfa(SegmentTransform &transform);
	static glm::quat getQuaternionTransformingFromTo(glm::vec3 from, glm::vec3 to);
	static std::shared_ptr<Segment> createSegment(std::shared_ptr<Segment> parent, SegmentTransform &transform, LindenmayerTreeMeshGenerator *meshGenerator);

	static glm::vec3 getFrontVector(glm::quat q) {
		//https://www.gamedev.net/forums/topic/56471-extracting-direction-glm::vectors-from-glm::quaternion/
		//Quat up glm::vec (tutle's front)
		return glm::vec3(2 * (q.x*q.y - q.w * q.z), 1 - 2 * (q.x*q.x + q.z * q.z), 2 * (q.y*q.z + q.w * q.x));
		//Quat front glm::vec
		//return glm::vec3(2 * (q.x*q.z + q.w * q.y), 2 * (q.y*q.z - q.w * q.x), 1 - 2 * (q.x*q.x + q.y * q.y));
	}

	static glm::vec3 getProperFrontVector(glm::quat q) {
		return glm::vec3(2 * (q.x*q.z + q.w * q.y), 2 * (q.y*q.z - q.w * q.x), 1 - 2 * (q.x*q.x + q.y * q.y));
	}

	static glm::vec3 getLeftVector(glm::quat q) {
		return glm::vec3(1 - 2 * (q.y*q.y + q.z*q.z), 2 * (q.x*q.y + q.w * q.z), 2 * (q.x*q.z - q.w * q.y));
	}

	static float toRadians(float angle) {
		return angle * 3.14159265359 / 180;
	}

	static float fromRadians(float radians) {
		return radians * 180 / 3.14159265359;
	}

	//Stuff
	static std::string number(float val) {
		if (val < 0.001 && val > -0.001)
			return "0";
		else
			return std::to_string(val);
	}

	static void printVec3(glm::vec3 val) {
		std::cout << number(val.x) + " " + number(val.y) + " " + number(val.z) << std::endl;
	}

	static void printMat4(glm::mat4 val) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				std::cout << number(val[j][i]) << " ";
			std::cout << std::endl;
		}
	}
};

#endif