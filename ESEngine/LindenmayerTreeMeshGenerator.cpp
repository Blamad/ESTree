#include "LindenmayerTreeMeshGenerator.h"

LindenmayerTreeMeshGenerator::LindenmayerTreeMeshGenerator(Mesh *mesh, int &ringDensity) : mesh(mesh), ringDensity(ringDensity) {
	this->textureXStep = 1.0f / (ringDensity - 1);
};

void LindenmayerTreeMeshGenerator::enqueueGenerationData(float &radius, mat4 &transform, int textureY) {
	float theta = 2 * pi<float>() / float(ringDensity - 1);
	int verticesOffset = mesh->vertices.size();

	for (int j = 0; j < ringDensity; j++) {
		mesh->vertices.push_back(Vertex());
		threadGenerationData.push_back(VertexGenerationAttributes(verticesOffset, j, theta, radius, transform, textureY));
	}
}

void LindenmayerTreeMeshGenerator::generateMesh() {
	int coresAvailable = boost::thread::hardware_concurrency();
	int numberOfVerticesPerCore = threadGenerationData.size() / coresAvailable;

	boost::thread_group group;
	for (int j = 0; j < coresAvailable - 1; j++)
		group.create_thread(boost::bind(&LindenmayerTreeMeshGenerator::computeRingPoint, this, numberOfVerticesPerCore*j, numberOfVerticesPerCore*(j + 1), this->mesh));
	group.create_thread(boost::bind(&LindenmayerTreeMeshGenerator::computeRingPoint, this, numberOfVerticesPerCore*(coresAvailable - 1), threadGenerationData.size(), this->mesh));
	group.join_all();
}

void LindenmayerTreeMeshGenerator::computeRingPoint(int startIndex, int endIndex, Mesh *mesh) {

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