#include "ModelExporter.h"

Logger ModelExporter::logger("ModelExporter");

void ModelExporter::exportToFile(vector<Renderable*> &renderables, string &filename) {
	map<Material, vector<Renderable*>> renderablesMap = mapRenderables(renderables);
	pair<Material, vector<Renderable*>> renderablePair;
	int matNumber = 0;
	int meshNumber = 0;
	int faceOffset = 0;

	boost::filesystem::path dir("models");
	if (boost::filesystem::create_directory(dir)) {
		logger.log(WARN, "\'models\' directory created");
	}

	//Init mtlfile
	ofstream mtlFile;
	mtlFile.open("models/" + filename + ".mtl");

	//Init objfile
	ofstream objFile;
	objFile.open("models/" + filename + ".obj");
	objFile << "#Model generated using ESTree\n";
	objFile << "mtllib " + filename + ".mtl\n";


	BOOST_FOREACH(renderablePair, renderablesMap) {
		Material mat = renderablePair.first;
		vector<Renderable*> meshes = renderablePair.second;

		//Insert material
		mtlFile << "newmtl material_" + toString(matNumber) + "\n";
		mtlFile << "Ns " + toString(mat.shininess) + "\n";
		mtlFile << "Ni 1.5000\n";
		mtlFile << "d 1.0000\n";
		mtlFile << "Tr 0.0000\n";
		mtlFile << "Tf 1.0000 1.0000 1.0000\n";
		mtlFile << "illum 2\n";
		if (mat.texSpecular != nullptr)
			mtlFile << "map_Ks " + processTextureFilename(mat.texSpecular->fileName) + "\n";
		if (mat.texDiffuse != nullptr)
			mtlFile << "map_Kd " + processTextureFilename(mat.texDiffuse->fileName) + "\n";

		objFile << "usemtl material_" + toString(matNumber++) + "\n";
		Renderable *renderable;
		BOOST_FOREACH(renderable, meshes) {
			objFile << "o mesh_" + toString(meshNumber++) + "\n";
			if (dynamic_cast<Mesh*>(renderable)) {
				Mesh* mesh = (Mesh*) renderable;
				BOOST_FOREACH(Vertex vertex, mesh->vertices) {
					objFile << "v " + toString(vertex.position.x) + " " + toString(vertex.position.y) + " " + toString(vertex.position.z) + "\n";
				}
				BOOST_FOREACH(Vertex vertex, mesh->vertices) {
					objFile << "vn " + toString(vertex.normal.x) + " " + toString(vertex.normal.y) + " " + toString(vertex.normal.z) + "\n";
				}
				BOOST_FOREACH(Vertex vertex, mesh->vertices) {
					objFile << "vt " + toString(vertex.texCoords.x) + " " + toString(vertex.texCoords.y) + "\n";
				}
				for (int i = 0; i < mesh->indices.size(); i += 6) {
					objFile << "f " + toFaceInfo(faceOffset + mesh->indices[i]) + " " + toFaceInfo(faceOffset + mesh->indices[i + 1]) + " " + toFaceInfo(faceOffset + mesh->indices[i + 4]) + " " + toFaceInfo(faceOffset + mesh->indices[i + 2]) + "\n";
				}
				faceOffset += mesh->vertices.size();
			}
			if (dynamic_cast<InstancedMesh*>(renderable)) {
				InstancedMesh* mesh = (InstancedMesh*)renderable;
				BOOST_FOREACH(InstancedTransform transform, mesh->instanceMatricies) {
					BOOST_FOREACH(Vertex vertex, mesh->vertices) {
						vec3 position = vec3(inverse(mesh->initialTransfom) * transform.modelMatrix * vec4(vertex.position, 1.0f));
						objFile << "v " + toString(position.x) + " " + toString(position.y) + " " + toString(position.z) + "\n";
					}
					BOOST_FOREACH(Vertex vertex, mesh->vertices) {
						vec3 normal = mat3(transpose(inverse(transform.modelMatrix))) * vertex.normal;
						objFile << "vn " + toString(normal.x) + " " + toString(normal.y) + " " + toString(normal.z) + "\n";
					}
					BOOST_FOREACH(Vertex vertex, mesh->vertices) {
						objFile << "vt " + toString(vertex.texCoords.x) + " " + toString(1.0f - vertex.texCoords.y) + "\n";
					}

					for (int i = 0; i < mesh->indices.size(); i += 6) {
						objFile << "f " + toFaceInfo(faceOffset + mesh->indices[i]) + " " + toFaceInfo(faceOffset + mesh->indices[i + 1]) + " " + toFaceInfo(faceOffset + mesh->indices[i + 4]) + " " + toFaceInfo(faceOffset + mesh->indices[i + 2]) + "\n";
					}
					faceOffset += mesh->vertices.size();
				}
			}
		}
	}

	mtlFile.close();
	objFile.close();
}

map<Material, vector<Renderable*>> ModelExporter::mapRenderables(vector<Renderable*> &renderables) {
	map<Material, vector<Renderable*>> renderablesMap;
	
	BOOST_FOREACH(Renderable *renderable, renderables) {
		Material material;

		if (dynamic_cast<Mesh*>(renderable)) {
			material = ((Mesh*)renderable)->material;
		}
		if (dynamic_cast<InstancedMesh*>(renderable)) {
			material = ((InstancedMesh*)renderable)->material;
		}

		if (!renderablesMap.count(material)) {
			renderablesMap[material] = vector<Renderable*>();
		}
		renderablesMap[material].push_back(renderable);
	}

	return renderablesMap;
}