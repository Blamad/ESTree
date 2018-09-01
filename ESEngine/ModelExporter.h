#ifndef MODEL_EXPORTER_H
#define MODEL_EXPORTER_H

#include <iostream>
#include <fstream>
#include <map>
#include <boost/foreach.hpp>
#include "Mesh.h"
#include "InstancedMesh.h"

using namespace std;

class ModelExporter {
public:
	static void exportToFile(vector<Renderable*> &renderables, string &filename);

private:
	static map<Material, vector<Renderable*>> mapRenderables(vector<Renderable*> &renderables);

	static string toString(float val) {
		if (val < 0.001 && val > -0.001)
			return "0";
		else
			return to_string(val);
	}

	static string toString(int val) {
		return to_string(val);
	}

	static string toFaceInfo(int val) {
		return toString(val + 1) + "/" + toString(val + 1) + "/" + toString(val + 1);
	}

	static string processTextureFilename(string val) {
		return val.substr(val.find_last_of('/') + 1);
	}

};

#endif