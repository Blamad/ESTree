#include "Tree.h"

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

void Tree::createMeshComponent() {
	vector<Vertex> v;
	vector<int> i;

	v.reserve(vBufferSize);
	i.reserve(iBufferSize);

	Shader shader("GenericShader.vert", "GenericShader.frag");
	mesh = shared_ptr<Mesh>(new Mesh(v, i, shader, vBufferSize, iBufferSize, GL_STREAM_DRAW));
	mesh->material = material;
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

Vertex Tree::createVertex(vec3 position, vec3 normal, vec2 texCoords) {
	Vertex vert;
	vert.position = position;
	vert.normal = normal;
	vert.texCoords = texCoords;
	vert.type[POSITION] = 1;
	vert.type[NORMAL] = 1;
	vert.type[TEXCOORDS] = 1;

	return vert;
}

//L-systems
///////////////////////////////

void Tree::createRing(float &radius, mat4 &transform) {
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

		vec4 tmpPosition = vec4(x * radius, 0, z* radius, 1);
		vec3 position = vec3(transform * tmpPosition);
		tmpPosition.z = 0;
		vec3 normal = normalize(vec3(transform * tmpPosition));
		
		//output verticies
		mesh->vertices.push_back(createVertex(position, normal));
	}
}

void Tree::createParallelRing(float &radius, mat4 &transform) {
	float theta = 2 * 3.1415926 / float(segments);
	int offset = mesh->vertices.size();

	boost::thread_group group;
	for (int j = 0; j < segments; j++) {
		mesh->vertices.push_back(Vertex());
		group.create_thread(boost::bind(&Tree::computeRingPoint, this, theta, radius, transform, offset, j));
	}
	group.join_all();
}

void Tree::computeRingPoint(float &theta, float &radius, mat4 &transform, int &offset, int index) {
	float sin = sinf(theta*index);
	float cos = cosf(theta*index);

	vec4 tmpPosition = vec4(radius * cos, 0, radius * sin, 1);
	vec3 position = vec3(transform * tmpPosition);
	tmpPosition.z = 0;
	vec3 normal = normalize(vec3(transform * tmpPosition));

	//output verticies
	Vertex vert = mesh->vertices[offset + index];
	vert.position = position;
	vert.normal = normal;
	vert.type[POSITION] = 1;
	vert.type[NORMAL] = 1;
}

void Tree::createRoot(float &radius, quat &rotation) {
	mat4 transform = mat4();
	if (rotation.w != 1)
		transform = transform * mat4_cast(rotation);

	createRing(radius, transform);
	root = shared_ptr<Segment>(new Segment());
	root->indiciesOffset = 0;
	root->verticiesOffset = 0;
	root->radius = radius;
	root->segments = segments;
	root->modelMatrix = transform;
}

shared_ptr<Segment> Tree::addSegment(shared_ptr<Segment> parent, float &radius, float &length, quat &rotation) {
	mat4 transform = mat4();
	if (rotation.w != 1)
		transform = transform * mat4_cast(rotation);
	transform = translate(transform, vec3(0, length, 0));
	

	shared_ptr<Segment> stem = shared_ptr<Segment>(new Segment(parent));
	stem->indiciesOffset = mesh->indices.size();
	stem->verticiesOffset = mesh->vertices.size();
	stem->modelMatrix = parent->modelMatrix * transform;
	stem->radius = radius;
	stem->segments = segments;

	createRing(stem->radius, stem->modelMatrix);
	linkSegmentWithParent(stem);
	parent->addChild(stem);

	return stem;
}

void Tree::createTexturedRoot(float &radius, quat &rotation) {
	mat4 transform = mat4();
	if (rotation.w != 1)
		transform = transform * mat4_cast(rotation);

	float theta = 2 * 3.1415926 / float(segments-1);
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

		vec4 tmpPosition = vec4(x * radius, 0, z* radius, 1);
		vec3 position = vec3(transform * tmpPosition);
		tmpPosition.z = 0;
		vec3 normal = normalize(vec3(transform * tmpPosition));

		//output verticies
		mesh->vertices.push_back(createVertex(position, normal));
	}

	root = shared_ptr<Segment>(new Segment());
	root->indiciesOffset = 0;
	root->verticiesOffset = 0;
	root->radius = radius;
	root->segments = segments;
	root->modelMatrix = transform;
}

shared_ptr<Segment> Tree::createSegment(shared_ptr<Segment> parent, float &radius, float &length, quat &rotation) {
	mat4 transform = mat4();
	if (rotation.w != 1)
		transform = transform * mat4_cast(rotation);
	transform = translate(transform, vec3(0, length, 0));

	//Create textured bottom ring:
	float textureStep = 1.0f/segments;
	//Copy end of last segment
	for (int i = 0; i < segments; i++)
	{
		if (parent->parent == nullptr) {
			mesh->vertices[parent->verticiesOffset + i].texCoords.x = i*textureStep;
			mesh->vertices[parent->verticiesOffset + i].texCoords.y = 0;
		} else {
			Vertex v = mesh->vertices[parent->verticiesOffset + i].copy();
			v.texCoords.x = i*textureStep;
			v.texCoords.y = 0;
			mesh->vertices.push_back(v);
		}
	}

	shared_ptr<Segment> stem = shared_ptr<Segment>(new Segment(parent));
	stem->indiciesOffset = mesh->indices.size();
	stem->verticiesOffset = mesh->vertices.size();
	stem->modelMatrix = parent->modelMatrix * transform;
	stem->radius = radius;
	stem->segments = segments;

	float theta = 2 * 3.1415926 / float(segments-1);
	float c = cosf(theta);
	float s = sinf(theta);
	float t;

	float x = radius;
	float z = 0;

	//Create textured top ring:
	for (int i = 0; i < segments; i++) {
		//apply the rotation matrix
		t = x;
		x = c * x - s * z;
		z = s * t + c * z;

		vec4 tmpPosition = vec4(x * radius, 0, z* radius, 1);
		vec3 position = vec3(stem->modelMatrix * tmpPosition);
		tmpPosition.z = 0;
		vec3 normal = normalize(vec3(stem->modelMatrix * tmpPosition));
		vec2 texCoords(i*textureStep, 1);

		//output verticies
		mesh->vertices.push_back(createVertex(position, normal, texCoords));
	}

	//Link two rings
	int bottomOffset = mesh->vertices.size() - 2 * segments;
	int topOffset = bottomOffset + segments;
	for (int i = 0; i < segments-1; i++) {
		mesh->indices.push_back(bottomOffset + i);
		mesh->indices.push_back(topOffset + i);
		mesh->indices.push_back(bottomOffset + i + 1);

		mesh->indices.push_back(topOffset + i);
		mesh->indices.push_back(topOffset + i + 1);
		mesh->indices.push_back(bottomOffset + i + 1);
	}

	parent->addChild(stem);

	return stem;
}