#include "LindenmayerTreeInterpreter.h"

boost::variate_generator<boost::mt19937, boost::uniform_real<>> LindenmayerTreeInterpreter::randomGenerator(boost::mt19937(time(0)), boost::uniform_real<>(0, 1));

std::vector<std::shared_ptr<Segment>> LindenmayerTreeInterpreter::generateMeshData(LindenmayerTreeParams &params, std::string product, LindenmayerTreeMeshGenerator *meshGenerator) {
	std::vector<std::shared_ptr<Segment>> segmentsVec;
	std::stack<std::shared_ptr<Segment>> segmentStack;
	std::stack<SegmentTransform> transformStack;

	std::shared_ptr<Segment> currentSegment;

	float customParameter = -1;

	SegmentTransform transform = SegmentTransform(glm::quat(), glm::quat(), params.initialLength, params.initialRadius, params.angle);
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
			transform.rotation *= angleAxis(customParameter, glm::vec3(1, 0, 0));
			break;
		case 'v': //pitch X axis -
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}
			transform.rotation *= angleAxis(-customParameter, glm::vec3(1, 0, 0));
			break;
		case '<': //yaw Z axis +
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= angleAxis(customParameter, glm::vec3(0, 0, 1));
			break;
		case '>': //yaw Z axis -
			customParameter = getNumericParameter(product, i);
			if (customParameter == -1)
				customParameter = transform.angle;
			else {
				i = returnNewIndexAfterParameter(product, i);
				customParameter = toRadians(customParameter);
			}

			transform.rotation *= angleAxis(-customParameter, glm::vec3(0, 0, 1));
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
			transform.rotation *= angleAxis(customParameter, glm::vec3(0, 1, 0));
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
			transform.rotation *= angleAxis(-customParameter, glm::vec3(0, 1, 0));
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

			transform = SegmentTransform(glm::quat(1.f, .0f, .0f, .0f), transform.totalRotation * transform.rotation, params.initialLength, transform.radius, transform.angle, transform.lengthScale, transform.roll);
			break;
		}
	}

	return segmentsVec;
}

std::shared_ptr<Segment> LindenmayerTreeInterpreter::createRoot(SegmentTransform &transform) {
	glm::mat4 segmentMatrix(1);
	if (transform.rotation.w != 1)
		segmentMatrix = segmentMatrix * mat4_cast(transform.rotation);

	std::shared_ptr<Segment> root = std::shared_ptr<Segment>(new Segment());
	root->indiciesOffset = 0;
	root->verticiesOffset = 0;
	root->radius = transform.radius;
	root->modelMatrix = segmentMatrix;

	return root;
}

float LindenmayerTreeInterpreter::returnNewIndexAfterParameter(std::string product, int index) {
	int endIndex = index;
	while (product.at(endIndex) != ')')
		endIndex++;
	return endIndex;
}

glm::quat LindenmayerTreeInterpreter::applyTropism(SegmentTransform &transform, glm::vec3 tropismDir, float bendingFactor) {
	glm::vec3 front = getFrontVector(transform.totalRotation * transform.rotation);
	glm::vec3 crossVec = cross(front, tropismDir);
	float angleVal = angle(front, tropismDir);
	angleVal *= sin(angleVal) * bendingFactor;

	glm::quat transformQuat = angleAxis(-transform.roll, glm::vec3(0, 1, 0)) * angleAxis(angleVal, crossVec) * angleAxis(transform.roll, glm::vec3(0, 1, 0));

	return transform.rotation * transformQuat;
}

glm::quat LindenmayerTreeInterpreter::restoreHorizontalOrientationAlfa(SegmentTransform &transform) {
	glm::vec3 worldUp(0, 1, 0);

	glm::vec3 up = getFrontVector(transform.totalRotation * transform.rotation);
	glm::vec3 left = normalize(cross(up, worldUp));
	left *= -1;
	glm::vec3 front = normalize(cross(up, left));
	front *= -1;

	glm::mat4 matrix = glm::make_mat4(new float[16]{
		left.x, left.y, left.z, 0,
		up.x, up.y, up.z, 0,
		front.x, front.y, front.z, 0,
		0, 0, 0, 0
		});

	glm::quat rotationQuat(matrix);
	return rotationQuat;
}

glm::quat LindenmayerTreeInterpreter::restoreHorizontalOrientation(SegmentTransform &transform) {
	glm::vec3 worldUp(0, -1, 0);

	glm::vec3 front = getFrontVector(transform.rotation);
	glm::vec3 left = normalize(cross(front, worldUp));
	glm::vec3 up = normalize(cross(front, left));

	float matrix[16] = {
		left.x, front.x, up.x, 0,
		left.y, front.y, up.y, 0,
		left.z, front.z, up.z, 0,
		0,		0,		 0,	   0
	};

	glm::mat4 rotationMatrix = glm::make_mat4(matrix);
	return glm::quat(rotationMatrix);
}

std::shared_ptr<Segment> LindenmayerTreeInterpreter::createSegment(std::shared_ptr<Segment> parent, SegmentTransform &segTransform, LindenmayerTreeMeshGenerator *meshGenerator) {
	glm::mat4 segmentMatrix(1);
	if (segTransform.rotation.w != 1)
		segmentMatrix = segmentMatrix * mat4_cast(segTransform.rotation);
	segmentMatrix = translate(segmentMatrix, glm::vec3(0, segTransform.length, 0));

	//Create textured bottom ring and rotate it to match top ring rotation:
	float roll = segTransform.roll - parent->roll;

	glm::mat4 alignmentTransform = parent->modelMatrix * mat4_cast(glm::quat(glm::vec3(0, roll, 0)));
	meshGenerator->enqueueGenerationData(parent->radius, alignmentTransform, 0);

	std::shared_ptr<Segment> stem = std::shared_ptr<Segment>(new Segment(parent));
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

float LindenmayerTreeInterpreter::getNumericParameter(std::string product, int index) {
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
			std::string strValue = product.substr(index + 2, offset - 2);
			value = atof(strValue.c_str());
		}
	}

	if (randomizeValue) {
		value = randomGenerator() * value;
		value = value;
	}

	return value;
}