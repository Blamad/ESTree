#ifndef SPLIT_H
#define SPLIT_H

#include "Segment.h"
#include <vector>

class Split : public Segment {
public:
	Split(shared_ptr<Segment> parent = nullptr) : Segment(parent) { };
	vector<shared_ptr<Segment>> children;

};

#endif