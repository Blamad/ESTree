#ifndef STEM_H
#define STEM_H

#include "Segment.h"

class Stem : public Segment {
public:
	Stem(shared_ptr<Segment> parent = nullptr) : Segment(parent) { };
	shared_ptr<Segment> child = nullptr;
};

#endif