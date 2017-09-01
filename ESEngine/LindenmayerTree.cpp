#include "LindenmayerTree.h"

boost::variate_generator<boost::mt19937, boost::uniform_real<> > LindenmayerTree::randomGenerator(boost::mt19937(time(0)), boost::uniform_real<>(0, 1));

LindenmayerTree::LindenmayerTree(LindenmayerTreeParams &params, Material &material, Material &leavesMaterial, bool useMeshWiring) : params(params), material(material), leavesMaterial(leavesMaterial), meshWiring(useMeshWiring), GameObject() {
	this->vBufferSize = 100000;
	this->iBufferSize = 4000000;
	this->segments = 10;
	this->textureXStep = 1.0f / (segments - 1);
}

void LindenmayerTree::generate() {
	createMeshComponent();
	generateTree();
}

void LindenmayerTree::generateTree() {
	generateMeshSkeleton();
	generateMeshData();
	generateInstancedLeaves();
	cout << "Vert: " << mesh->vertices.size() << "\nInd:" << mesh->indices.size() << "\nTris:" << mesh->indices.size() / 3 << endl;
}

//PARSING L DATA

void LindenmayerTree::generateMeshSkeleton() {

	product = params.axiom;

	//Generate tree structure from axiom and rules
	string newProduct;
	for (int i = 0; i < params.depth; i++) {
		newProduct.clear();
		for (int j = 0; j < product.size(); j++) {
			string currentSymbol = string(1, product.at(j));
			newProduct += parseRule(currentSymbol, i);
		}
		product = newProduct;
	}
}

string LindenmayerTree::parseRule(string &symbol, int &depth) {
	vector<Rule> rules;
	float randomSpace = 0;
	BOOST_FOREACH(Rule rule, params.getRules(symbol)) {
		if (rule.allowedDepth <= depth) {
			rules.push_back(rule);
			randomSpace += rule.probability;
		}
	}

	//Only one rule
	if (rules.size() == 1)
		return rules[0].replacement;
	//There goes some random stuff
	if (rules.size() > 1) {
		float value = randomGenerator() * randomSpace;
		for (int i = 0; i < rules.size(); i++) {
			value -= rules[i].probability;
			if (value <= 0)
				return rules[i].replacement;
		}
	}
	//No rules available
	return symbol;
}

float LindenmayerTree::getNumericParameter(string product, int index) {
	float value = -1;

	if (product.length() > (index + 1) && product.at(index + 1) == '(') {
		int offset = 0;
		while (product.length() > (index + offset) && product.at(index + offset) != ')')
			offset++;
		value = atof(product.substr(index + 2, offset - 2).c_str());
	}
	return value;
}

float LindenmayerTree::returnNewIndexAfterParameter(string product, int index) {
	int endIndex = index;
	while (product.at(endIndex) != ')')
		endIndex++;
	return endIndex;
}

//MESH AND VERTICES STUFF

void LindenmayerTree::createMeshComponent() {
	vector<Vertex> v;
	vector<int> i;

	v.reserve(vBufferSize);
	i.reserve(iBufferSize);

	vector<Shader> shaders;
	shaders.push_back(Shader("GenericShader.vert", "GenericShader.frag"));
	//if (meshWiring)
		shaders.push_back(Shader("MeshWiringShader.vert", "MeshWiringShader.frag", "MeshWiringShader.geom"));

	mesh = shared_ptr<Mesh>(new Mesh(v, i, shaders, vBufferSize, iBufferSize, GL_STREAM_DRAW));
	mesh->material = material;
	addComponent(mesh);
}

void LindenmayerTree::generateMeshData() {

	//Translate results of generation to mesh
	stack<shared_ptr<Segment>> segmentStack;
	stack<SegmentTransform> transformStack;

	shared_ptr<Segment> currentSegment;

	float customParameter = -1;

	SegmentTransform transform = SegmentTransform(quat(), params.initialLength, params.initialRadius, 1, 0);
	createRoot(transform);
	currentSegment = root;
	segmentsVec.push_back(currentSegment);

	for (int i = 0; i < product.size(); i++) {
		char character = product.at(i);
		switch (character) {
		case '[':
			segmentStack.push(currentSegment);
			transformStack.push(SegmentTransform(transform.rotation, transform.length, transform.radius, transform.lengthScale, transform.yawRotation));
			break;
		case ']':
			currentSegment = segmentStack.top();
			segmentStack.pop();
			transform = transformStack.top();
			transformStack.pop();
			break;
		case '-': //turn left
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= angleAxis(-customParameter, vec3(0, 0, 1));
			break;
		case '+': //turn right
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= angleAxis(customParameter, vec3(0, 0, 1));
			break;
		case '\\': //roll clockwise
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= angleAxis(customParameter, vec3(1, 0, 0));
			break;
		case '/': //roll counterclockwise
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= angleAxis(-customParameter, vec3(1, 0, 0));
			break;
		case '^': //pitch up
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.yawRotation += customParameter;
			transform.rotation *= angleAxis(customParameter, vec3(0, 1, 0));
			break;
		case 'v': //pitch down
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.yawRotation -= customParameter;
			transform.rotation *= angleAxis(-customParameter, vec3(0, 1, 0));
			break;
		case '\"': //scale down
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = 0.625;
			else
				i = returnNewIndexAfterParameter(product, i);

			transform.lengthScale *= customParameter;
			transform.radius *= customParameter;
			break;
		case '=': //scale thickness
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = 0.625;
			else
				i = returnNewIndexAfterParameter(product, i);

			transform.radius *= customParameter;
			break;
		case ' ':
		default:
			break;
		case 'f':
		case 'F':
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.length;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter *= transform.lengthScale;
			}

			transform.length = customParameter;
			currentSegment = createSegment(currentSegment, transform);
			segmentsVec.push_back(currentSegment);
			transform = SegmentTransform(quat(), transform.length, transform.radius, transform.lengthScale, transform.yawRotation);
			break;
		}
	}
	//TODO usun to pozniej!
	generateVertices();
	mesh->updateMesh();
}

void LindenmayerTree::createRoot(SegmentTransform &transform) {
	mat4 segmentMatrix = mat4();
	if (transform.rotation.w != 1)
		segmentMatrix = segmentMatrix * mat4_cast(transform.rotation);

	root = shared_ptr<Segment>(new Segment());
	root->indiciesOffset = 0;
	root->verticiesOffset = 0;
	root->radius = transform.radius;
	root->segments = 0;
	root->modelMatrix = segmentMatrix;
}

Vertex LindenmayerTree::createVertex(vec3 position, vec3 normal, vec2 texCoords) {
	Vertex vert;
	vert.position = position;
	vert.normal = normal;
	vert.texCoords = texCoords;
	vert.type[POSITION] = 1;
	vert.type[NORMAL] = 1;
	vert.type[TEXCOORDS] = 1;

	return vert;
}

shared_ptr<Segment> LindenmayerTree::createSegment(shared_ptr<Segment> parent, SegmentTransform &segTransform) {
	mat4 segmentMatrix = mat4();
	if (segTransform.rotation.w != 1)
		segmentMatrix = segmentMatrix * mat4_cast(segTransform.rotation);
	segmentMatrix = translate(segmentMatrix, vec3(0, segTransform.length, 0));

	//Create textured bottom ring and rotate it to match top ring rotation:
	float yaw = segTransform.yawRotation - parent->yawRotation;
	
	mat4 alignmentTransform = parent->modelMatrix * mat4_cast(quat(vec3(0, yaw, 0)));
	enqueueGenerationData(parent->radius, alignmentTransform, 0);

	shared_ptr<Segment> stem = shared_ptr<Segment>(new Segment(parent));
	stem->indiciesOffset = mesh->indices.size();
	stem->verticiesOffset = mesh->vertices.size();
	stem->modelMatrix = parent->modelMatrix * segmentMatrix;
	stem->radius = segTransform.radius;
	stem->segments = segments;
	stem->yawRotation = segTransform.yawRotation;

	//Create top ring:
	enqueueGenerationData(stem->radius, stem->modelMatrix, 1);

	//Link two rings
	int bottomOffset = mesh->vertices.size() - 2 * segments;
	int topOffset = bottomOffset + segments;
	for (int i = 0; i < segments - 1; i++) {
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

//PARALLEL RING COMPUTATIONS

void LindenmayerTree::enqueueGenerationData(float &radius, mat4 &transform, int textureY) {
	float theta = 2 * pi<float>() / float(segments - 1);
	int verticesOffset = mesh->vertices.size();

	for (int j = 0; j < segments; j++) {
		mesh->vertices.push_back(Vertex());
		threadGenerationData.push_back(VertexGenerationAttributes(verticesOffset, j, theta, radius, transform, textureY));
	}
}

void LindenmayerTree::generateVertices() {
	int coresAvailable = boost::thread::hardware_concurrency();
	int numberOfVerticesPerCore = threadGenerationData.size() / coresAvailable;

	boost::thread_group group;
	for (int j = 0; j < coresAvailable - 1; j++) 
		group.create_thread(boost::bind(&LindenmayerTree::computeRingPoint, this, numberOfVerticesPerCore*j, numberOfVerticesPerCore*(j+1)));
	group.create_thread(boost::bind(&LindenmayerTree::computeRingPoint, this, numberOfVerticesPerCore*(coresAvailable - 1), threadGenerationData.size()));
	group.join_all();
}

void LindenmayerTree::computeRingPoint(int startIndex, int endIndex) {

	float arg, sin, cos;
	vec4 tmpPosition;
	vec3 position, normal;
	Vertex *vert;
	VertexGenerationAttributes *vga;

	for (int i = startIndex; i < endIndex; i++) {
		vga = &threadGenerationData[i];
		arg = vga->theta*vga->vertexNumber;
		sin = sinf(arg) / 2;
		cos = cosf(arg) / 2;

		tmpPosition = vec4(vga->radius * cos, 0, vga->radius * sin, 1);
		position = vec3(vga->transform * tmpPosition);
		tmpPosition.z = 0;
		normal = normalize(vec3(vga->transform * tmpPosition));

		//output verticies
		vert = &mesh->vertices[vga->verticesOffset + vga->vertexNumber];
		vert->position = position;
		vert->normal = normal;
		vert->texCoords = vec2(textureXStep * vga->vertexNumber, vga->textureY);
		vert->type[POSITION] = 1;
		vert->type[NORMAL] = 1;
		vert->type[TEXCOORDS] = 1;
	}
}

// LEAVES

void LindenmayerTree::generateInstancedLeaves() {
	Transform *transform = (Transform*)getComponent(TRANSFORM);
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