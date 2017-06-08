#ifndef TREE_H
#define TREE_H
#include <vector>

#include "Mesh.h"
#include "Vertex.h"
#include "Shader.h"
#include "GameObject.h"
#include "Material.h"

#include "Segment.h"
#include "Split.h"
#include "Stem.h"


using namespace glm;

class Tree : public GameObject {
public:
	Tree();

protected:
	shared_ptr<Mesh> mesh;
	shared_ptr<Stem> root;

	int vBufferSize = 1000;
	int iBufferSize = 4000;
	int segments = 30;

	shared_ptr<Stem> addStem(shared_ptr<Stem> parent, float radius, vec3 center, vec3 rotation);
	shared_ptr<Split> splitStem(shared_ptr<Stem> parent, float radius, vec3 position, vec3 rotation);
	//W przyszlosci kazda galaz bedzie musiala miec nowa podstawe, nie ta sama ktora ma split.
	shared_ptr<Stem> addBranch(shared_ptr<Split> parent, float radius, vec3 position, vec3 rotation);
	void linkSegmentWithParent(shared_ptr<Segment> segment);
	void createRing(float radius, vec3 center, quat rotation);
	void createRoot(float radius, vec3 center);

	void createMesh();
	void generateTree(vec3 startPos);

	Vertex createVertex(vec3 position, vec3 normal);
};

#endif