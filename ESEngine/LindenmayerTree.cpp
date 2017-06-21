#include "LindenmayerTree.h"

boost::variate_generator<boost::mt19937, boost::uniform_real<> > LindenmayerTree::randomGenerator(boost::mt19937(time(0)), boost::uniform_real<>(0, 1));

LindenmayerTree::LindenmayerTree(LindenmayerTreeParams &params) : params(params) {
	this->vBufferSize = 100000;
	this->iBufferSize = 4000000;
	this->segments = 10;

	createMeshComponent();
	generateTree();
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

void LindenmayerTree::generateTree() {
	generateProduct();
	generateMesh();
	cout << "Vert: " << mesh->vertices.size() << endl << "Ind:" << mesh->indices.size() << endl;
}