#ifndef MODEL_EXPORTER_H
#define MODEL_EXPORTER_H

#include <iostream>
#include <fstream>
#include <map>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include "Mesh.h"
#include "InstancedMesh.h"
#include "Logger.h"

class ModelExporter {
public:
	static void exportToFile(std::vector<Renderable*> &renderables, std::string &filename);

private:
	static Logger logger;

	static std::map<Material, std::vector<Renderable*>> mapRenderables(std::vector<Renderable*> &renderables);

	static std::string toString(float val) {
		if (val < 0.001 && val > -0.001)
			return "0";
		else
			return std::to_string(val);
	}

	static std::string toString(int val) {
		return std::to_string(val);
	}

	static std::string toFaceInfo(int val) {
		return toString(val + 1) + "/" + toString(val + 1) + "/" + toString(val + 1);
	}

	static std::string processTextureFilename(std::string val) {
		return val.substr(val.find_last_of('/') + 1);
	}

};

#endif