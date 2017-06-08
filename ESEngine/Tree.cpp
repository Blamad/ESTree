#include "Tree.h"

Tree::Tree() : GameObject() {
	createMesh();
	generateTree(vec3(0, 0, 2));
}

void Tree::generateTree(vec3 pos) {
	float z = pos.z;
	float x = pos.x;
	segments = 20;

	createRoot(.6f, vec3(x, 0, z));
	shared_ptr<Stem> stem;
	shared_ptr<Split> split;

	stem = addStem(root, .5f, vec3(x, 2, z), vec3(0, 0, 0));
	stem = addStem(stem, .4f, vec3(x, 3, z), vec3(0, 0, 0));
	split = splitStem(stem, .35f, vec3(x, 3.5f, z), vec3(0, 0, 0));
	stem = addBranch(split, .01f, vec3(x-1.5f, 5, z), vec3(-0.5, 0, 0));
	stem = addBranch(split, .01f, vec3(x, 6.5f, z), vec3(0, 0, 0));
	stem = addBranch(split, .01f, vec3(x + 1.5f, 5, z), vec3(0.5, 0, 0));
	
	mesh->updateMesh();
}

shared_ptr<Stem> Tree::addBranch(shared_ptr<Split> parent, float radius, vec3 position, vec3 rotation) {
	shared_ptr<Stem> stem = shared_ptr<Stem>(new Stem(parent));
	stem->indiciesOffset = mesh->indices.size();
	stem->verticiesOffset = mesh->vertices.size();
	stem->position = position;
	stem->rotation = angleAxis(radians(90.0f), rotation);
	stem->radius = radius;
	stem->segments = segments;

	createRing(stem->radius, stem->position, stem->rotation);
	linkSegmentWithParent(stem);
	parent->children.push_back(stem);

	return stem;
}

shared_ptr<Split> Tree::splitStem(shared_ptr<Stem> parent, float radius, vec3 position, vec3 rotation) {
	shared_ptr<Split> split = shared_ptr<Split>(new Split(parent));
	split->indiciesOffset = mesh->indices.size();
	split->verticiesOffset = mesh->vertices.size();
	split->position = position;
	split->rotation = angleAxis(radians(90.0f), rotation);
	split->radius = radius;
	split->segments = segments;

	createRing(split->radius, split->position, split->rotation);
	linkSegmentWithParent(split);
	parent->child = split;

	return split;
}

shared_ptr<Stem> Tree::addStem(shared_ptr<Stem> parent, float radius, vec3 position, vec3 rotation) {
	shared_ptr<Stem> stem = shared_ptr<Stem>(new Stem(parent));
	stem->indiciesOffset = mesh->indices.size();
	stem->verticiesOffset = mesh->vertices.size();
	stem->position = position;
	stem->rotation = angleAxis(radians(90.0f), rotation);
	stem->radius = radius;
	stem->segments = segments;

	createRing(stem->radius, stem->position, stem->rotation);
	linkSegmentWithParent(stem);
	parent->child = stem;

	return stem;
}

void Tree::linkSegmentWithParent(shared_ptr<Segment> segment) {
	shared_ptr<Segment> parent = segment->parent;
	for (int i = 0; i < segments; i++) {
		mesh->indices.push_back(parent->verticiesOffset + i);
		mesh->indices.push_back(segment->verticiesOffset + i);
		mesh->indices.push_back(parent->verticiesOffset + i + 1);

		mesh->indices.push_back(segment->verticiesOffset + i);
		mesh->indices.push_back(segment->verticiesOffset + i + 1);
		mesh->indices.push_back(parent->verticiesOffset + i + 1);
	}

	mesh->indices[segment->indiciesOffset + segment->segments * 6 - 1] = mesh->indices[segment->indiciesOffset];
	mesh->indices[segment->indiciesOffset + segment->segments * 6 - 2] = mesh->indices[segment->indiciesOffset + 1];
	mesh->indices[segment->indiciesOffset + segment->segments * 6 - 4] = mesh->indices[segment->indiciesOffset];
}

void Tree::createRoot(float radius, vec3 pos) {
	createRing(radius, pos, quat(1, 0, 0, 0));
	root = shared_ptr<Stem>(new Stem());
	root->indiciesOffset = 0;
	root->verticiesOffset = 0;
	root->position = vec3(0, 0, 0);
	root->rotation = quat(1, 0, 0, 0);
	root->radius = radius;
	root->segments = segments;
}

void Tree::createRing(float radius, vec3 center, quat rotation) {
	float theta = 2 * 3.1415926 / float(segments);
	float c = cosf(theta);
	float s = sinf(theta);
	float t;

	float x = radius;
	float z = 0;

	for (int i = 0; i < segments; i++) {
		//apply the rotation matrix
		t = x;
		x = c * x - s * z;
		z = s * t + c * z;

		vec3 position = vec3(x * radius, 0, z* radius);
		position = vec3(rotation * vec4(position, 1));
		position = position + vec3(center.x, center.y, center.z);
		vec3 normal = normalize(vec3(rotation * vec4(x, 0, z, 0)));

		//output verticies
		mesh->vertices.push_back(createVertex(position, normal));
	}
}

void Tree::createMesh() {
	vector<Vertex> v;
	vector<int> i;

	v.reserve(vBufferSize);
	i.reserve(iBufferSize);

	Shader shader("GenericShader.vert", "GenericShader.frag");
	mesh = shared_ptr<Mesh>(new Mesh(v, i, shader, vBufferSize, iBufferSize, GL_STREAM_DRAW));
	mesh->material = Material::copper();
	addComponent(mesh);
}

Vertex Tree::createVertex(vec3 position, vec3 normal) {
	Vertex vert;
	vert.position = position;
	vert.normal = normal;
	vert.type[POSITION] = 1;
	vert.type[NORMAL] = 1;

	return vert;
}
