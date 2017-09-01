#ifndef TREE_H
#define TREE_H

#include "Mesh.h"
#include "Vertex.h"
#include "Shader.h"
#include "GameObject.h"
#include "Material.h"
#include "Segment.h"

#include <vector>
#include <boost/thread/thread.hpp>

using namespace glm;

class Tree : public GameObject {
public:
	Tree(Material material) : material(material), GameObject() { }

protected:
	Material material;
	shared_ptr<Mesh> mesh;
	shared_ptr<Segment> root;

	int vBufferSize = 1000;
	int iBufferSize = 4000;
	int segments = 15;

	void createMeshComponent();

	void createRoot(float &radius, quat &rotation);
	void createRing(float &radius, mat4 &transform);
	shared_ptr<Segment> addSegment(shared_ptr<Segment> parent, float &radius, float &length, quat &rotation);
	
	void linkSegmentWithParent(shared_ptr<Segment> segment);
	Vertex createVertex(vec3 position, vec3 normal);
	Vertex createVertex(vec3 position, vec3 normal, vec2 texCoords);

	//Parallel ring
	void createParallelRing(float &radius, mat4 &transform);
	void computeRingPoint(float &theta, float &radius, mat4 &transform, int &offset, int &index);

	//Textured segments
	void createTexturedRoot(float &radius, quat &rotation);
	shared_ptr<Segment> createSegment(shared_ptr<Segment> parent, float &radius, float &length, quat &rotation);
};

#endif