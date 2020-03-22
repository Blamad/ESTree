#ifndef LINDENMAYER_TREE_H
#define LINDENMAYER_TREE_H

#include <string>
#include <vector>

#include <time.h>
#include <boost/generator_iterator.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "ShaderManager.h"
#include "Segment.h"
#include "Mesh.h"
#include "InstancedMesh.h"
#include "LindenmayerTreeParams.h"
#include "LindenmayerTreeSolver.h"
#include "LindenmayerTreeMeshGenerator.h"
#include "LindenmayerTreeInterpreter.h"

#include "Logger.h"

class LindenmayerTree : public GameObject {
public:
	LindenmayerTree(LindenmayerTreeParams &params, Material &material = Material::bark5(), Material &leavesMaterial = Material::leaves2(), bool useMeshWiring = false, bool normalVisualisation = false);
	void generate();
	bool meshWiring;
	bool normalVisualisation;

	LindenmayerTreeParams getTreeParams() { return params; }
	Material getBarkMaterial() { return barkMaterial; };
	Material getLeavesMaterial() { return leavesMaterial; }

protected:
	Material barkMaterial;
	Material leavesMaterial;
	LindenmayerTreeParams params;

	std::shared_ptr<Mesh> mesh;
	std::vector<std::shared_ptr<Segment>> segmentsVec;

	int vBufferSize = 1000;
	int iBufferSize = 4000;
	const float branchLeafOverlappingFactor = .8f;
	
	int ringDensity;

private:
	static Logger logger;

	std::unique_ptr<LindenmayerTreeMeshGenerator> meshGenerator;
	std::string product;

	static boost::variate_generator<boost::mt19937, boost::uniform_real<>> randomGenerator;

	void generateTree();

	//Parsing l-data
	void generateMeshDescription();
	
	//Creating mesh generation data
	void generateMeshData();
	
	//Mesh and vertices stuff
	void generateTreeMesh();
	void createMeshComponent();
	
	void generateLeaves();
	std::shared_ptr<Mesh> generateLeaf();

	//Leaves generation
	void generateInstancedLeaves();
	std::shared_ptr<InstancedMesh> createInstancedLeavesPanelMesh(std::vector<InstancedTransform> &instancedTransforms);
	
	//Stuff
	std::string number(float val) {
		if (val < 0.001 && val > -0.001)
			return "0";
		else
			return std::to_string(val);
	}

	void printVec3(glm::vec3 val) {
		std::cout << number(val.x) + " " + number(val.y) + " " + number(val.z) << std::endl;
	}

	void printMat4(glm::mat4 val) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				std::cout << number(val[j][i]) << " ";
			std::cout << std::endl;
		}
	}
};

#endif