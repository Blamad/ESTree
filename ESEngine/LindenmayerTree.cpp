#include "LindenmayerTree.h"
#include <glm/gtx/matrix_decompose.hpp>

boost::variate_generator<boost::mt19937, boost::uniform_real<> > LindenmayerTree::randomGenerator(boost::mt19937(time(0)), boost::uniform_real<>(0, 1));

Logger LindenmayerTree::logger("LindenmayerTree");

LindenmayerTree::LindenmayerTree(LindenmayerTreeParams &params, Material &material, Material &leavesMaterial, bool useMeshWiring, bool normalVisualisation) : params(params), material(material), leavesMaterial(leavesMaterial), meshWiring(useMeshWiring), normalVisualisation(normalVisualisation), GameObject() {
	this->vBufferSize = 100000;
	this->iBufferSize = 4000000;
	this->segments = 9;
	this->textureXStep = 1.0f / (segments - 1);
}

void LindenmayerTree::generate() {
	createMeshComponent();
	generateTree();
}

void LindenmayerTree::generateTree() {
	logger.log(INFO, "Generation started: " + params.name);
	generateMeshSkeleton();
	generateMeshData();
	//generateInstancedLeaves();
	//generateLeaves();
	logger.log(INFO, "Generation finished. " + to_string(mesh->indices.size() / 3) + " tris.");
}

//PARSING L DATA
void LindenmayerTree::generateMeshSkeleton() {
	LindenmayerTreeGenerator parser(params);
	product = parser.generateTreeProduction();
	logger.log(INFO, product);
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
		case '^': //rotate branch in x axis
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}
			transform.rotation *= angleAxis(customParameter, vec3(1, 0, 0));
			break;
		case 'v':
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}
			transform.rotation *= angleAxis(-customParameter, vec3(1, 0, 0));
			break;
		case '>': //rotate branch in z axis
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= angleAxis(customParameter, vec3(0, 0, 1));
			break;
		case '<':
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= angleAxis(-customParameter, vec3(0, 0, 1));
			break;
		case '+': //twist branch up/down in y axis
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
		case '-':
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
			/*customParameter = transform.roll;
			customParameter *= -1;
			
			transform.roll += customParameter;
			transform.rotation *= angleAxis(customParameter, vec3(0, 1, 0));*/

			transform.roll = 0;
			transform.rotation = test(transform);

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
			transform = SegmentTransform(quat(), params.initialLength, transform.radius, transform.lengthScale, transform.roll);
			break;
		}
	}
	generateVertices();
	mesh->updateMesh();
}

quat LindenmayerTree::test(SegmentTransform transform) {
	mat4 rotationMatrix = mat4_cast(transform.rotation);
	
	cout << "Matrix:" << endl;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			cout << rotationMatrix[j][i] << " ";
		cout << endl;
	}

	vec3 front = transform.rotation * vec3(0, 1, 0);
	vec3 worldUp = vec3(0, 1, 0);

	vec3 left = normalize(cross(front, worldUp));
	vec3 up = normalize(cross(front, left));

	//front *= -1;
	//left *= -1;

	rotationMatrix[0][0] = left.x;
	rotationMatrix[1][0] = left.y;
	rotationMatrix[2][0] = left.z;

	rotationMatrix[0][1] = up.x;
	rotationMatrix[1][1] = up.y;
	rotationMatrix[2][1] = up.z;

	rotationMatrix[0][2] = front.x;
	rotationMatrix[1][2] = front.y;
	rotationMatrix[2][2] = front.z;

	return quat(rotationMatrix);
}

float LindenmayerTree::calculateRollAngle(shared_ptr<Segment> parent, SegmentTransform &transform) {
	mat4 segmentMatrix = mat4();
	if (transform.rotation.w != 1)
		segmentMatrix = segmentMatrix * mat4_cast(transform.rotation);
	segmentMatrix = translate(segmentMatrix, vec3(0, transform.length, 0));
	mat4 modelMatrix = parent->modelMatrix * segmentMatrix;

	float x, y, z;
	extractEulerAngleXYZ(modelMatrix, x, y, z);

	return y;
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
	enqueueGenerationData(parent->radius, alignmentTransform, 0);

	shared_ptr<Segment> stem = shared_ptr<Segment>(new Segment(parent));
	stem->indiciesOffset = mesh->indices.size();
	stem->verticiesOffset = mesh->vertices.size();
	stem->modelMatrix = parent->modelMatrix * segmentMatrix;
	stem->radius = segTransform.radius;
	stem->segments = segments;
	stem->roll = segTransform.roll;

	/*vec3 leftVec(stem->modelMatrix[0][0], stem->modelMatrix[0][1], stem->modelMatrix[0][2]);
	logger.log(INFO, "R vec: (" + to_string(leftVec.x) + "; " + to_string(leftVec.y) + "; " + to_string(leftVec.z) + ")");

	vec3 headingVec(stem->modelMatrix[0][2], stem->modelMatrix[1][2], stem->modelMatrix[2][2]);
	logger.log(INFO, "H vec: (" + to_string(headingVec.x) + "; " + to_string(headingVec.y) + "; " + to_string(headingVec.z) + ")");

	vec3 upVec(stem->modelMatrix[0][1], stem->modelMatrix[1][1], stem->modelMatrix[2][1]);
	logger.log(INFO, "U vec: (" + to_string(upVec.x) + "; " + to_string(upVec.y) + "; " + to_string(upVec.z) + ")");*/

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

		mat3 rotationMatrix(vga->transform);

		tmpPosition = vec4(vga->radius * cos, 0, vga->radius * sin, 1);
		position = vec3(vga->transform * tmpPosition);
		normal = normalize(rotationMatrix * vec3(tmpPosition) * vec3(2, 2, 2));

		//output verticies
		vert = &mesh->vertices[vga->verticesOffset + vga->vertexNumber];
		vert->position = position;
		vert->normal = normal;
		vert->texCoords = vec2(textureXStep * vga->vertexNumber, vga->textureY);
	}
}

// LEAVES

void LindenmayerTree::generateLeaves() {
	for (auto & seg : segmentsVec) {
		if (!seg->isLastStem())
			continue;
		//if (randomGenerator() > 0.33f) {
		if (randomGenerator() > 0.0f) {
			shared_ptr<GameObject> go (new GameObject());
			Transform* leafTransform = (Transform*) go->getComponent(TRANSFORM);
			
			leafTransform->setModelMatrix(seg->modelMatrix);
			/*glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(seg->modelMatrix, scale, rotation, translation, skew, perspective);
			rotation = glm::conjugate(rotation);
			
			leafTransform->translate(translation);
			leafTransform->rotate(rotation);*/

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
		if (randomGenerator() > 0.0f) {
			InstancedTransform it;
			//TODO przemieszczenie lisci.
			//it.modelMatrix = glm::translate(it.modelMatrix, vec3(0, branchLeafOverlappingFactor, 0));
			//it.modelMatrix = it.modelMatrix * transform->getModelMatrix() * seg->modelMatrix;
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