#ifndef SEGMENT_H
#define SEGMENT_H

#include <vector>
#include <memory>

#include <GLM/glm.hpp>
#include <GLM/gtc/quaternion.hpp>

enum SegmentType {
	STEM, SPLIT
};

class SegmentTransform {
public:
	SegmentTransform(glm::quat &rotation, glm::quat &totalRotation, float length, float radius, float angle, float lengthScale = 1, float roll = 0) : rotation(rotation), totalRotation(totalRotation), length(length), radius(radius), lengthScale(lengthScale), roll(roll), angle(angle) {};
	float length;
	float lengthScale;
	float radius;
	float angle;
	
	glm::quat rotation;
	glm::quat totalRotation;
	float roll;
};

class Segment {
public:
	Segment(std::shared_ptr<Segment> parent = nullptr) : parent(parent) { };
	
	void addChild(std::shared_ptr<Segment> child) {
		children.push_back(child);
		type = children.size() == 1 ? STEM : SPLIT;
	}

	std::shared_ptr<Segment> parent;
	std::vector<std::shared_ptr<Segment>> children;

	bool isLastStem() {
		return children.empty();
	}

	SegmentType type;

	//Buffer  offsets
	int verticiesOffset = -1;
	int indiciesOffset = -1;
	//Segment radius scale
	float radius = 0;
	//Rotation
	glm::quat rotation;
	//Position
	glm::vec3 position;

	//Transform matrix
	glm::mat4 modelMatrix;

	float roll;
};

#endif