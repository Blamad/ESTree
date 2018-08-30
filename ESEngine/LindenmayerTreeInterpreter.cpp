#include "LindenmayerTreeInterpreter.h"

boost::variate_generator<boost::mt19937, boost::uniform_real<> > LindenmayerTreeInterpreter::randomGenerator(boost::mt19937(time(0)), boost::uniform_real<>(0, 1));

vector<shared_ptr<Segment>> LindenmayerTreeInterpreter::generateMeshData(LindenmayerTreeParams &params, string product, LindenmayerTreeMeshGenerator *meshGenerator) {
	vector<shared_ptr<Segment>> segmentsVec;
	stack<shared_ptr<Segment>> segmentStack;
	stack<SegmentTransform> transformStack;

	shared_ptr<Segment> currentSegment;

	float customParameter = -1;

	SegmentTransform transform = SegmentTransform(quat(), quat(), params.initialLength, params.initialRadius, params.angle);
	currentSegment = createRoot(transform);
	segmentsVec.push_back(currentSegment);

	for (int i = 0; i < product.size(); i++) {
		char character = product.at(i);
		switch (character) {
		case '[':
			segmentStack.push(currentSegment);
			transformStack.push(SegmentTransform(transform.rotation, transform.totalRotation, transform.length, transform.radius, transform.angle, transform.lengthScale, transform.roll));
			break;
		case ']':
			currentSegment = segmentStack.top();
			segmentStack.pop();
			transform = transformStack.top();
			transformStack.pop();
			break;
		case '^': //pitch X axis +
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}
			transform.rotation *= angleAxis(customParameter, vec3(1, 0, 0));
			break;
		case 'v': //pitch X axis -
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}
			transform.rotation *= angleAxis(-customParameter, vec3(1, 0, 0));
			break;
		case '<': //yaw Z axis +
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= angleAxis(customParameter, vec3(0, 0, 1));
			break;
		case '>': //yaw Z axis -
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= angleAxis(-customParameter, vec3(0, 0, 1));
			break;
		case '+': //roll Y axis +
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.roll += customParameter;
			transform.rotation *= angleAxis(customParameter, vec3(0, 1, 0));
			break;
		case '-': //roll Y axis -
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.roll -= customParameter;
			transform.rotation *= angleAxis(-customParameter, vec3(0, 1, 0));
			break;
		case ';': //scale angle
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = 0.625;
			else
				i = returnNewIndexAfterParameter(product, i);

			transform.angle *= customParameter;
			break;
		case '`': //scale radius and length
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
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = params.tropismBendingFactor;
			else
				i = returnNewIndexAfterParameter(product, i);
			transform.rotation = applyTropism(transform, params.tropism, customParameter);
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
			currentSegment = createSegment(currentSegment, transform, meshGenerator);
			segmentsVec.push_back(currentSegment);

			transform = SegmentTransform(quat(), transform.totalRotation * transform.rotation, params.initialLength, transform.radius, transform.angle, transform.lengthScale, transform.roll);
			break;
		}
	}

	return segmentsVec;
}

shared_ptr<Segment> LindenmayerTreeInterpreter::createRoot(SegmentTransform &transform) {
	mat4 segmentMatrix = mat4();
	if (transform.rotation.w != 1)
		segmentMatrix = segmentMatrix * mat4_cast(transform.rotation);

	shared_ptr<Segment> root = shared_ptr<Segment>(new Segment());
	root->indiciesOffset = 0;
	root->verticiesOffset = 0;
	root->radius = transform.radius;
	root->modelMatrix = segmentMatrix;

	return root;
}

float LindenmayerTreeInterpreter::returnNewIndexAfterParameter(string product, int index) {
	int endIndex = index;
	while (product.at(endIndex) != ')')
		endIndex++;
	return endIndex;
}

quat LindenmayerTreeInterpreter::applyTropism(SegmentTransform &transform, vec3 tropismDir, float bendingFactor) {
	vec3 front = getFrontVector(transform.totalRotation * transform.rotation);
	vec3 crossVec = cross(front, tropismDir);
	float angleVal = angle(front, tropismDir);
	angleVal *= sin(angleVal) * bendingFactor;

	quat transformQuat = angleAxis(-transform.roll, vec3(0, 1, 0)) * angleAxis(angleVal, crossVec) * angleAxis(transform.roll, vec3(0, 1, 0));

	return transform.rotation * transformQuat;
}

quat LindenmayerTreeInterpreter::restoreHorizontalOrientationAlfa(SegmentTransform &transform) {
	vec3 worldUp = vec3(0, 1, 0);

	vec3 up = getFrontVector(transform.totalRotation * transform.rotation);
	vec3 left = normalize(cross(up, worldUp));
	left *= -1;
	vec3 front = normalize(cross(up, left));
	front *= -1;

	mat4 matrix = make_mat4(new float[16]{
		left.x, left.y, left.z, 0,
		up.x, up.y, up.z, 0,
		front.x, front.y, front.z, 0,
		0, 0, 0, 0
		});

	quat rotationQuat = quat(matrix);
	return rotationQuat;
}

quat LindenmayerTreeInterpreter::restoreHorizontalOrientation(SegmentTransform &transform) {
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

shared_ptr<Segment> LindenmayerTreeInterpreter::createSegment(shared_ptr<Segment> parent, SegmentTransform &segTransform, LindenmayerTreeMeshGenerator *meshGenerator) {
	mat4 segmentMatrix = mat4();
	if (segTransform.rotation.w != 1)
		segmentMatrix = segmentMatrix * mat4_cast(segTransform.rotation);
	segmentMatrix = translate(segmentMatrix, vec3(0, segTransform.length, 0));

	//Create textured bottom ring and rotate it to match top ring rotation:
	float roll = segTransform.roll - parent->roll;

	mat4 alignmentTransform = parent->modelMatrix * mat4_cast(quat(vec3(0, roll, 0)));
	meshGenerator->enqueueGenerationData(parent->radius, alignmentTransform, 0);

	shared_ptr<Segment> stem = shared_ptr<Segment>(new Segment(parent));
	stem->indiciesOffset = meshGenerator->getIndicesLength();
	stem->verticiesOffset = meshGenerator->getVerticesLength();
	stem->modelMatrix = parent->modelMatrix * segmentMatrix;
	stem->radius = segTransform.radius;
	stem->roll = segTransform.roll;

	//Enqueue top ring creation:
	meshGenerator->enqueueGenerationData(stem->radius, stem->modelMatrix, 1);

	//Link two rings
	meshGenerator->createIndiciesForSegment();

	parent->addChild(stem);

	return stem;
}

float LindenmayerTreeInterpreter::getNumericParameter(string product, int index) {
	float value = -1;
	bool randomizeValue = false;
	if (product.length() > (index + 1)) {
		if (product.at(index + 1) == '~') {
			randomizeValue = true;
			index++;
		}

		if (product.at(index + 1) == '(') {
			int offset = 0;
			while (product.length() > (index + offset) && product.at(index + offset) != ')')
				offset++;
			string strValue = product.substr(index + 2, offset - 2);
			value = atof(strValue.c_str());
		}
	}

	if (randomizeValue) {
		value = randomGenerator() * value;
		value = value;
	}

	return value;
}