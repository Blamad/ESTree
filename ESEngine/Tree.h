#ifndef TREE_H
#define TREE_H
#include <vector>
#include <boost/thread/thread.hpp>

#include "Mesh.h"
#include "Vertex.h"
#include "Shader.h"
#include "GameObject.h"
#include "Material.h"

#include "Segment.h"

using namespace glm;

class Tree : public GameObject {
public:
	Tree();

protected:
	shared_ptr<Mesh> mesh;
	shared_ptr<Segment> root;

	int vBufferSize = 1000;
	int iBufferSize = 4000;
	int segments = 30;

	void createMeshComponent();

	void createRoot(float &radius, quat &rotation);
	void createRing(float &radius, mat4 &transform);
	shared_ptr<Segment> addSegment(shared_ptr<Segment> parent, float &radius, float &length, quat &rotation);
	
	void linkSegmentWithParent(shared_ptr<Segment> segment);
	Vertex createVertex(vec3 position, vec3 normal);

	//Parallel ring
	void createParallelRing(float &radius, mat4 &transform);
	void computeRingPoint(float &theta, float &radius, mat4 &transform, int &offset, int index);
};

#endif