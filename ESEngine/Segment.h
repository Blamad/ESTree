#ifndef SEGMENT_H
#define SEGMENT_H

#include <GLM\glm.hpp>

using namespace std;

class Segment {
public:
	Segment(shared_ptr<Segment> parent = nullptr) : parent(parent) { };
	shared_ptr<Segment> parent;

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
};

#endif