#ifndef SEGMENT_H
#define SEGMENT_H

#include <vector>
#include <memory>

#include <GLM/glm.hpp>
#include <GLM/gtc/quaternion.hpp>

using namespace std;

enum SegmentType {
	STEM, SPLIT
};

class SegmentTransform {
public:
	SegmentTransform(glm::quat &rotation, float length, float radius, float lengthScale = 1, float roll = 0) : rotation(rotation), length(length), radius(radius), lengthScale(lengthScale), roll(roll) {};
	float length;
	float lengthScale;
	float radius;
	
	glm::quat rotation;
	float roll;
};

class Segment {
public:
	Segment(shared_ptr<Segment> parent = nullptr) : parent(parent) { };
	
	void addChild(shared_ptr<Segment> child) {
		children.push_back(child);
		type = children.size() == 1 ? STEM : SPLIT;
	}

	shared_ptr<Segment> parent;
	vector<shared_ptr<Segment>> children;

	bool isLastStem() {
		return children.empty();
	}

	SegmentType type;

	//Buffer  offsets
	int verticiesOffset = -1;
	int indiciesOffset = -1;
	//Number of segments
	int segments = -1;
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