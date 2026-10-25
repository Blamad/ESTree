#include "LindenmayerTreeInterpreter.h"

#include <treegen/Error.h>
#include <treegen/Module.h>
#include <treegen/Skeleton.h>
#include <treegen/Turtle.h>

#include "Engine/Core/Logger.h"

static Logger logger("LindenmayerTreeInterpreter");

std::vector<std::shared_ptr<Segment>> LindenmayerTreeInterpreter::generateMeshData(LindenmayerTreeParams &params, std::string product, LindenmayerTreeMeshGenerator *meshGenerator) {
	treegen::InterpretOptions options;
	options.seed = params.seed;

	treegen::Skeleton skeleton;
	try {
		skeleton = treegen::interpret(params, treegen::parseModules(product), options);
	} catch (const treegen::Error &error) {
		// The legacy interpreter silently skipped what it did not understand. An exception must not leave the render
		// thread, so the tree degenerates to its root.
		logger.log(LOG_ERROR, "Tree interpretation failed for " + params.name + ": " + error.what());
		skeleton = treegen::interpret(params, treegen::ModuleString());
	}

	std::vector<std::shared_ptr<Segment>> segmentsVec;
	segmentsVec.reserve(skeleton.segments.size());

	// segments[0] is the root (no rings), the others are in creation order, i.e. every parent precedes its children:
	// the rings are enqueued in exactly the order the old interpreter created them.
	for (size_t i = 0; i < skeleton.segments.size(); i++) {
		const treegen::SkeletonSegment &source = skeleton.segments[i];

		if (i == 0) {
			std::shared_ptr<Segment> root = std::make_shared<Segment>();
			root->indiciesOffset = 0;
			root->verticiesOffset = 0;
			root->radius = source.radius;
			root->modelMatrix = source.model;
			root->roll = source.roll;
			segmentsVec.push_back(root);
			continue;
		}

		std::shared_ptr<Segment> parent = segmentsVec[source.parent];

		//Create textured bottom ring and rotate it to match top ring rotation:
		float roll = source.roll - parent->roll;

		glm::mat4 alignmentTransform = parent->modelMatrix * mat4_cast(glm::quat(glm::vec3(0, roll, 0)));
		meshGenerator->enqueueGenerationData(parent->radius, alignmentTransform, 0);

		std::shared_ptr<Segment> stem = std::shared_ptr<Segment>(new Segment(parent));
		stem->indiciesOffset = meshGenerator->getIndicesLength();
		stem->verticiesOffset = meshGenerator->getVerticesLength();
		stem->modelMatrix = source.model;
		stem->radius = source.radius;
		stem->roll = source.roll;

		//Enqueue top ring creation:
		meshGenerator->enqueueGenerationData(stem->radius, stem->modelMatrix, 1);

		//Link two rings
		meshGenerator->createIndiciesForSegment();

		parent->addChild(stem);
		segmentsVec.push_back(stem);
	}

	return segmentsVec;
}
