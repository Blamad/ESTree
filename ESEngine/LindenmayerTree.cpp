#include "LindenmayerTree.h"
#include <glm/gtx/matrix_decompose.hpp>

float LindenmayerTree::leavesGrowthProbability = 0.0f;

boost::variate_generator<boost::mt19937, boost::uniform_real<> > LindenmayerTree::randomGenerator(boost::mt19937(time(0)), boost::uniform_real<>(0, 1));

Logger LindenmayerTree::logger("LindenmayerTree");

LindenmayerTree::LindenmayerTree(LindenmayerTreeParams &params, Material &material, Material &leavesMaterial, bool useMeshWiring, bool normalVisualisation) : params(params), material(material), leavesMaterial(leavesMaterial), meshWiring(useMeshWiring), normalVisualisation(normalVisualisation), GameObject() {
	this->vBufferSize = 100000;
	this->iBufferSize = 4000000;
	this->ringDensity = 9;
}

void LindenmayerTree::generate() {
	createMeshComponent();
	generateTree();
}

void LindenmayerTree::generateTree() {
	logger.log(INFO, "Generation started: " + params.name);
	generateTreeMesh();
	//generateInstancedLeaves();
	generateLeaves();
	logger.log(INFO, "Generation finished. " + to_string(mesh->indices.size() / 3) + " tris.");
}

void LindenmayerTree::generateTreeMesh() {
	generateMeshSkeleton();
	generateMeshData();
}

//PARSING L DATA
void LindenmayerTree::generateMeshSkeleton() {
	product = LindenmayerTreeSolver::generateTreeProduction(params);
}

float LindenmayerTree::getNumericParameter(string product, int index) {
	float value = -1;

	if (product.length() > (index + 1) && product.at(index + 1) == '(') {
		int offset = 0;
		while (product.length() > (index + offset) && product.at(index + offset) != ')')
			offset++;
		string strValue = product.substr(index + 2, offset - 2);
		value = atof(strValue.c_str());
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


	Shader shader = Shader("Shaders/GenericShader.vert", "Shaders/GenericShader.frag");
	mesh = shared_ptr<Mesh>(new Mesh(v, i, shader, vBufferSize, iBufferSize, GL_STREAM_DRAW));

	if (meshWiring)
		mesh->showMeshWiring();
	if (normalVisualisation)
		mesh->showNormalVisualisation();

	mesh->material = material;
	addComponent(mesh);

	meshGenerator = make_unique<LindenmayerTreeMeshGenerator>(LindenmayerTreeMeshGenerator(mesh.get(), ringDensity));
}

void LindenmayerTree::generateMeshData() {

	//Translate results of generation to mesh
	stack<shared_ptr<Segment>> segmentStack;
	stack<SegmentTransform> transformStack;

	shared_ptr<Segment> currentSegment;

	float customParameter = -1;

	SegmentTransform transform = SegmentTransform(quat(), params.initialLength, params.initialRadius);
	createRoot(transform);
	currentSegment = root;
	segmentsVec.push_back(currentSegment);

	for (int i = 0; i < product.size(); i++) {
		char character = product.at(i);
		switch (character) {
		case '[':
			segmentStack.push(currentSegment);
			transformStack.push(SegmentTransform(transform.rotation, transform.length, transform.radius, transform.lengthScale, transform.roll));
			break;
		case ']':
			currentSegment = segmentStack.top();
			segmentStack.pop();
			transform = transformStack.top();
			transformStack.pop();
			break;
		case 'v': //pitch branch in x axis
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}
			transform.rotation *= angleAxis(customParameter, vec3(1, 0, 0));
			break;
		case '^':
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}
			transform.rotation *= angleAxis(-customParameter, vec3(1, 0, 0));
			break;
		case '<': //yaw branch in z axis
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= angleAxis(customParameter, vec3(0, 0, 1));
			break;
		case '>':
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= angleAxis(-customParameter, vec3(0, 0, 1));
			break;
		case '-': //roll branch in y axis
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.roll -= customParameter;
			transform.rotation *= angleAxis(-customParameter, vec3(0, 1, 0));
			break;
		case '+':
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.roll += customParameter;
			transform.rotation *= angleAxis(customParameter, vec3(0, 1, 0));
			break;
		case '`': //scale down
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
		case '$':
			transform.rotation = restoreHorizontalOrientation(transform);
			transform.roll = roll(transform.rotation);
			break;
		case 'T':
			transform.rotation = applyTropism(transform);
			transform.roll = roll(transform.rotation);
			break;
		case ' ':
		default:
			break;
		case 'f':
		case 'F':
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.length;
			else
				i = returnNewIndexAfterParameter(product, i);

			transform.length = customParameter * transform.lengthScale;
			currentSegment = createSegment(currentSegment, transform);
			segmentsVec.push_back(currentSegment);
			if (true) {
				transform.rotation = applyTropism(transform);
				transform.roll = roll(transform.rotation);
			}

			transform = SegmentTransform(quat(), params.initialLength, transform.radius, transform.lengthScale, transform.roll);
			break;
		}
	}
	meshGenerator->generateMesh();
	mesh->updateMesh();
}

quat LindenmayerTree::applyTropism(SegmentTransform transform) {
	vec3 front = getFrontVector(transform.rotation);
	vec3 newFront = front + params.tropism * params.tropismBendingFactor;
	vec3 rotationVector = normalize(cross(front, newFront));
	float rotationAngle = angle(front, newFront);

	return transform.rotation * angleAxis(rotationAngle, rotationVector);
	//return mix(transform.rotation, params.tropism, params.tropismBendingFactor);
}
 
quat LindenmayerTree::restoreHorizontalOrientation(SegmentTransform transform) {
	vec3 worldUp = vec3(0, -1, 0);

	vec3 front = getFrontVector(transform.rotation);
	vec3 left = normalize(cross(front, worldUp));
	vec3 up = normalize(cross(front, left));

	float matrix[16] = {
		left.x, front.x, up.x, 0,
		left.y, front.y, up.y, 0,
		left.z, front.z, up.z, 0,
		0,		0,		 0,	   0
	};

	mat4 rotationMatrix = make_mat4(matrix);
	return quat(rotationMatrix);
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

shared_ptr<Segment> LindenmayerTree::createSegment(shared_ptr<Segment> parent, SegmentTransform &segTransform) {
	mat4 segmentMatrix = mat4();
	if (segTransform.rotation.w != 1)
		segmentMatrix = segmentMatrix * mat4_cast(segTransform.rotation);
	segmentMatrix = translate(segmentMatrix, vec3(0, segTransform.length, 0));

	//Create textured bottom ring and rotate it to match top ring rotation:
	float roll = segTransform.roll - parent->roll;
	
	mat4 alignmentTransform = parent->modelMatrix * mat4_cast(quat(vec3(0, roll, 0)));
	meshGenerator->enqueueGenerationData(parent->radius, alignmentTransform, 0);

	shared_ptr<Segment> stem = shared_ptr<Segment>(new Segment(parent));
	stem->indiciesOffset = mesh->indices.size();
	stem->verticiesOffset = mesh->vertices.size();
	stem->modelMatrix = parent->modelMatrix * segmentMatrix;
	stem->radius = segTransform.radius;
	stem->segments = ringDensity;
	stem->roll = segTransform.roll;

	//Create top ring:
	meshGenerator->enqueueGenerationData(stem->radius, stem->modelMatrix, 1);

	//Link two rings
	int bottomOffset = mesh->vertices.size() - 2 * ringDensity;
	int topOffset = bottomOffset + ringDensity;
	for (int i = 0; i < ringDensity - 1; i++) {
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

// LEAVES

void LindenmayerTree::generateLeaves() {
	for (auto & seg : segmentsVec) {
		if (!seg->isLastStem())
			continue;
		if (randomGenerator() > leavesGrowthProbability) {
			shared_ptr<GameObject> go(new GameObject());
			Transform* leafTransform = (Transform*)go->getComponent(TRANSFORM);

			if (params.leavesAngleDiversity > 0) { 
				mat4 leafRotation;
				float angle = randomGenerator() * 2 * params.leavesAngleDiversity - params.leavesAngleDiversity;
				leafRotation = rotate(leafRotation, angle, vec3(0,1,0));
				leafTransform->setModelMatrix(leafRotation * seg->modelMatrix);
			}
			else
				leafTransform->setModelMatrix(seg->modelMatrix);
			

			shared_ptr<Mesh> leafMesh = generateLeaf();
			go->addComponent(leafMesh);

			this->addGameObject(go);
		}
	}
}

shared_ptr<Mesh> LindenmayerTree::generateLeaf() {
	Shader shader("Shaders/GenericShader.vert", "Shaders/GenericShader.frag");

	vector<Vertex> vertices = {
		//front
		Vertex::createVertex(vec3(1.0f, -1.0f,  0.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 1)),
		Vertex::createVertex(vec3(1.0f,  1.0f,  0.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 0)),
		Vertex::createVertex(vec3(-1.0f,  1.0f,  0.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 0)),
		Vertex::createVertex(vec3(-1.0f, -1.0f,  0.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 1)),
		//back
		Vertex::createVertex(vec3(-1.0f, -1.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(0, 1)),
		Vertex::createVertex(vec3(-1.0f,  1.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(0, 0)),
		Vertex::createVertex(vec3(1.0f,  1.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 0)),
		Vertex::createVertex(vec3(1.0f, -1.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 1)),
	};

	vector<int> indices = {
		0, 1, 3, 1, 2, 3,
		4, 5, 7, 5, 6, 7,
	};

	shared_ptr<Mesh> mesh = shared_ptr<Mesh>(new Mesh(vertices, indices, shader, vertices.size(), indices.size(), GL_STREAM_DRAW));
	mesh->material = leavesMaterial;
	return mesh;
}

void LindenmayerTree::generateInstancedLeaves() {
	Transform *transform = (Transform*)getComponent(TRANSFORM);
	vector<InstancedTransform> instancedTransforms;
	for (auto & seg : segmentsVec) {
		if (!seg->isLastStem())
			continue;
		if (randomGenerator() > leavesGrowthProbability) {
			InstancedTransform it;

			if (params.leavesAngleDiversity > 0) {
				mat4 leafRotation;
				float angle = randomGenerator() * 2 * params.leavesAngleDiversity - params.leavesAngleDiversity;
				leafRotation = rotate(leafRotation, angle, vec3(0, 1, 0));
				it.modelMatrix = leafRotation * transform->getModelMatrix() * seg->modelMatrix;
			}
			else
				it.modelMatrix = transform->getModelMatrix() * seg->modelMatrix;
			
			it.generateNormalModelMatrix();
			instancedTransforms.push_back(it);
		}
	}

	shared_ptr<InstancedMesh> leavesMesh = createInstancedLeavesPanelMesh(instancedTransforms);
	addComponent(leavesMesh);
}

shared_ptr<InstancedMesh> LindenmayerTree::createInstancedLeavesPanelMesh(vector<InstancedTransform> &instancedTransforms) {
	Shader shader("Shaders/InstancedShader.vert", "Shaders/GenericShader.frag");

	vector<Vertex> vertices = {
		//front
		Vertex::createVertex(vec3(1.0f, -1.0f,  0.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 1)),
		Vertex::createVertex(vec3(1.0f,  1.0f,  0.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 0)),
		Vertex::createVertex(vec3(-1.0f,  1.0f,  0.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 0)),
		Vertex::createVertex(vec3(-1.0f, -1.0f,  0.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 1)),
		//back
		Vertex::createVertex(vec3(-1.0f, -1.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(0, 1)),
		Vertex::createVertex(vec3(-1.0f,  1.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(0, 0)),
		Vertex::createVertex(vec3(1.0f,  1.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 0)),
		Vertex::createVertex(vec3(1.0f, -1.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 1)),
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
	Shader shader("Shaders/InstancedShader.vert", "Shaders/GenericShader.frag");

	vector<Vertex> vertices = {
		//front
		Vertex::createVertex(vec3(1.0f, -1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 0)),
		Vertex::createVertex(vec3(1.0f,  1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(1, 1)),
		Vertex::createVertex(vec3(-1.0f,  1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 1)),
		Vertex::createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(0.0f, 0.0f, 1.0f),		vec2(0, 0)),
		//back
		Vertex::createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),	vec2(0, 0)),
		Vertex::createVertex(vec3(-1.0f,  1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(0, 1)),
		Vertex::createVertex(vec3(1.0f,  1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 1)),
		Vertex::createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(0.0f, 0.0f, -1.0f),	vec2(1, 0)),
		//left
		Vertex::createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(-1.0f, 0.0f, 0.0f),	vec2(0, 1)),
		Vertex::createVertex(vec3(-1.0f,  1.0f,  1.0f),	vec3(-1.0f, 0.0f, 0.0f),	vec2(1, 1)),
		Vertex::createVertex(vec3(-1.0f,  1.0f, -1.0f),	vec3(-1.0f, 0.0f, 0.0f),	vec2(1, 0)),
		Vertex::createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(-1.0f, 0.0f, 0.0f),	vec2(0, 0)),
		//right
		Vertex::createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(0, 0)),
		Vertex::createVertex(vec3(1.0f,  1.0f, -1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(1, 0)),
		Vertex::createVertex(vec3(1.0f,  1.0f,  1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(1, 1)),
		Vertex::createVertex(vec3(1.0f, -1.0f,  1.0f),	vec3(1.0f, 0.0f, 0.0f),		vec2(0, 1)),
		//top
		Vertex::createVertex(vec3(1.0f, 1.0f,  1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(1, 1)),
		Vertex::createVertex(vec3(1.0f, 1.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(1, 0)),
		Vertex::createVertex(vec3(-1.0f, 1.0f, -1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(0, 0)),
		Vertex::createVertex(vec3(-1.0f, 1.0f,  1.0f),	vec3(0.0f, 1.0f, 0.0f),		vec2(0, 1)),
		//bottom
		Vertex::createVertex(vec3(-1.0f, -1.0f,  1.0f),	vec3(0.0f, -1.0f, 0.0f),	vec2(0, 1)),
		Vertex::createVertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f),	vec2(0, 0)),
		Vertex::createVertex(vec3(1.0f, -1.0f, -1.0f),	vec3(0.0f, -1.0f, 0.0f),	vec2(1, 0)),
		Vertex::createVertex(vec3(1.0f, -1.0f,  1.0f),	vec3(0.0f, -1.0f, 0.0f),	vec2(1, 1)),
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