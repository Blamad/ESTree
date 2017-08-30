#include "LindenmayerTree.h"

boost::variate_generator<boost::mt19937, boost::uniform_real<> > LindenmayerTree::randomGenerator(boost::mt19937(time(0)), boost::uniform_real<>(0, 1));

LindenmayerTree::LindenmayerTree(LindenmayerTreeParams &params, Material &material, Material &leavesMaterial) : params(params), leavesMaterial(leavesMaterial), Tree(material) {
	this->vBufferSize = 100000;
	this->iBufferSize = 4000000;
	this->segments = 10;
}

void LindenmayerTree::generate() {
	createMeshComponent();
	generateTexturedTree();
}

void LindenmayerTree::generateProduct() {

	product = params.axiom;

	//Generate tree structure from axiom and rules
	string newProduct;
	for (int i = 0; i < params.depth; i++) {
		newProduct.clear();
		for (int j = 0; j < product.size(); j++) {
			string currentSymbol = string(1, product.at(j));
			newProduct += parseRule(currentSymbol);
		}
		product = newProduct;
	}
}

string LindenmayerTree::parseRule(string &symbol) {
	vector<Rule> rules = params.getRules(symbol);
	//Only one rule
	if (rules.size() == 1)
		return rules[0].replacement;
	//There goes some random stuff
	if (rules.size() > 1) {
		float value = randomGenerator();
		for (int i = 0; i < rules.size(); i++) {
			value -= rules[i].probability;
			if (value <= 0)
				return rules[i].replacement;
		}
	}
	//No rules available
	return symbol;
}

void LindenmayerTree::generateMesh() {

	//Translate results of generation to mesh
	stack<shared_ptr<Segment>> segmentStack;
	stack<SegmentTransform> transformStack;

	shared_ptr<Segment> currentSegment;

	SegmentTransform transform = SegmentTransform(quat(), params.initialLength, params.initialRadius);
	createRoot(transform.radius, transform.rotation);
	currentSegment = root;

	for (int i = 0; i < product.size(); i++) {
		char character = product.at(i);
		switch (character) {
		case '[':
			segmentStack.push(currentSegment);
			transformStack.push(SegmentTransform(transform.rotation, transform.length, transform.radius));
			break;
		case ']':
			currentSegment = segmentStack.top();
			segmentStack.pop();
			transform = transformStack.top();
			transformStack.pop();
			break;
		case '\\': //roll clockwise
		case '-': //turn left
			transform.rotation *= angleAxis(-params.angle, vec3(0, 0, 1));
			break;
		case '/': //roll counterclockwise
		case '+': //turn right
			transform.rotation *= angleAxis(params.angle, vec3(0, 0, 1));
			break;
		case '&': //pitch up
			transform.rotation *= angleAxis(params.angle, vec3(1, 0, 0));
			break;
		case '^': //pitch down
			transform.rotation *= angleAxis(-params.angle, vec3(1, 0, 0));
			break;
		case '\"': //scale down
			transform.length *= float(4) / 5;
			transform.radius *= float(4) / 5;
			break;
		case ' ':
			break;
		default:
			currentSegment = addSegment(currentSegment, transform.radius, transform.length, transform.rotation);
			transform = SegmentTransform(quat(), transform.length, transform.radius);
			break;
		}
	}
	mesh->updateMesh();
}

void LindenmayerTree::generateTexturedMesh() {

	//Translate results of generation to mesh
	stack<shared_ptr<Segment>> segmentStack;
	stack<SegmentTransform> transformStack;

	shared_ptr<Segment> currentSegment;

	SegmentTransform transform = SegmentTransform(quat(), params.initialLength, params.initialRadius);
	createTexturedRoot(transform.radius, transform.rotation);
	currentSegment = root;
	segmentsVec.push_back(currentSegment);

	for (int i = 0; i < product.size(); i++) {
		char character = product.at(i);
		switch (character) {
		case '[':
			segmentStack.push(currentSegment);
			transformStack.push(SegmentTransform(transform.rotation, transform.length, transform.radius));
			break;
		case ']':
			currentSegment = segmentStack.top();
			segmentStack.pop();
			transform = transformStack.top();
			transformStack.pop();
			break;
		case '\\': //roll clockwise
		case '-': //turn left
			transform.rotation *= angleAxis(-params.angle, vec3(0, 0, 1));
			break;
		case '/': //roll counterclockwise
		case '+': //turn right
			transform.rotation *= angleAxis(params.angle, vec3(0, 0, 1));
			break;
		case '&': //pitch up
			transform.rotation *= angleAxis(params.angle, vec3(1, 0, 0));
			break;
		case '^': //pitch down
			transform.rotation *= angleAxis(-params.angle, vec3(1, 0, 0));
			break;
		case '\"': //scale down
			transform.length *= float(4) / 5;
			transform.radius *= float(4) / 5;
			break;
		case ' ':
			break;
		default:
			currentSegment = createSegment(currentSegment, transform.radius, transform.length, transform.rotation);
			segmentsVec.push_back(currentSegment);
			transform = SegmentTransform(quat(), transform.length, transform.radius);
			break;
		}
	}
	mesh->updateMesh();
}

void LindenmayerTree::generateTree() {
	generateProduct();
	generateMesh();
	cout << "Vert: " << mesh->vertices.size() << endl << "Ind:" << mesh->indices.size() << endl;
}

void LindenmayerTree::generateTexturedTree() {
	generateProduct();
	generateTexturedMesh();
	generateInstancedLeaves();
	cout << "Vert: " << mesh->vertices.size() << "\nInd:" << mesh->indices.size() << "\nTris:" << mesh->indices.size() / 3 << endl;
}

void LindenmayerTree::generateInstancedLeaves() {
	Transform *transform = (Transform*) getComponent(TRANSFORM);
	vector<InstancedTransform> instancedTransforms;
	for (auto & seg : segmentsVec) {
		if (!seg->isLastStem())
			continue;
		if (randomGenerator() > 0.0f) {
			InstancedTransform it;
			it.modelMatrix = transform->getModelMatrix() * seg->modelMatrix;
			it.generateNormalModelMatrix();
			instancedTransforms.push_back(it);
		}
	}

	shared_ptr<InstancedMesh> leavesMesh = createInstancedLeavesPanelMesh(instancedTransforms);
	addComponent(leavesMesh);
}

shared_ptr<InstancedMesh> LindenmayerTree::createInstancedLeavesPanelMesh(vector<InstancedTransform> &instancedTransforms) {
	Shader shader("InstancedShader.vert", "InstancedShader.frag");

	vector<Vertex> vertices = {
		//front
		createVertex(vec3(1.0f, -1.0f,  0.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 1)),
		createVertex(vec3(1.0f,  1.0f,  0.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 0)),
		createVertex(vec3(-1.0f,  1.0f,  0.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 0)),
		createVertex(vec3(-1.0f, -1.0f,  0.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 1)),
		//back
		createVertex(vec3(-1.0f, -1.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(0, 1)),
		createVertex(vec3(-1.0f,  1.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(0, 0)),
		createVertex(vec3(1.0f,  1.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 0)),
		createVertex(vec3(1.0f, -1.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 1)),
	};

	vector<int> indices = {
		0, 1, 3, 1, 2, 3,
		4, 5, 7, 5, 6, 7,
	};

	shared_ptr<InstancedMesh> mesh = shared_ptr<InstancedMesh>(new InstancedMesh(vertices, indices, instancedTransforms, shader, vertices.size(), indices.size(), GL_STREAM_DRAW));
	mesh->material = leavesMaterial;
	return mesh;
}

shared_ptr<InstancedMesh> LindenmayerTree::createInstancedLeavesCubeMesh(vector<InstancedTransform> &instancedTransforms) {
	Shader shader("InstancedShader.vert", "InstancedShader.frag");

	vector<Vertex> vertices = {
		//front
		createVertex(vec3(1.0f, -1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 0)),
		createVertex(vec3(1.0f,  1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 1)),
		createVertex(vec3(-1.0f,  1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 1)),
		createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 0)),
		//back
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),	vec2(0, 0)),
		createVertex(vec3(-1.0f,  1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(0, 1)),
		createVertex(vec3(1.0f,  1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 1)),
		createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 0)),
		//left
		createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(-1.0f, 0.0f, 0.0f),	vec2(0, 1)),
		createVertex(vec3(-1.0f,  1.0f,  1.0f),	vec3(-1.0f, 0.0f, 0.0f),	vec2(1, 1)),
		createVertex(vec3(-1.0f,  1.0f, -1.0f),	vec3(-1.0f, 0.0f, 0.0f),	vec2(1, 0)),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(-1.0f, 0.0f, 0.0f),	vec2(0, 0)),
		//right
		createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(0, 0)),
		createVertex(vec3(1.0f,  1.0f, -1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(1, 0)),
		createVertex(vec3(1.0f,  1.0f,  1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(1, 1)),
		createVertex(vec3(1.0f, -1.0f,  1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(0, 1)),
		//top
		createVertex(vec3(1.0f, 1.0f,  1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(1, 1)),
		createVertex(vec3(1.0f, 1.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(1, 0)),
		createVertex(vec3(-1.0f, 1.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(0, 0)),
		createVertex(vec3(-1.0f, 1.0f,  1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(0, 1)),
		//bottom
		createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(0.0f, -1.0f, 0.0f),	vec2(0, 1)),
		createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f),	vec2(0, 0)),
		createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(0.0f, -1.0f, 0.0f),	vec2(1, 0)),
		createVertex(vec3(1.0f, -1.0f,  1.0f),	vec3(0.0f, -1.0f, 0.0f),	vec2(1, 1)),
	};

	vector<int> indices = {
		0, 1, 3, 1, 2, 3,
		4, 5, 7, 5, 6, 7,
		8, 9, 11, 9, 10, 11,
		12, 13, 15, 13, 14, 15,
		16, 17, 19, 17, 18, 19,
		20, 21, 23, 21, 22, 23
	};

	shared_ptr<InstancedMesh> mesh = shared_ptr<InstancedMesh>(new InstancedMesh(vertices, indices, instancedTransforms, shader, vertices.size(), indices.size(), GL_STREAM_DRAW));
	mesh->material = leavesMaterial;
	return mesh;
}