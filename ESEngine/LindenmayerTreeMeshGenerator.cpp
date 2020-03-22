#include "LindenmayerTreeMeshGenerator.h"

LindenmayerTreeMeshGenerator::LindenmayerTreeMeshGenerator(Mesh *mesh, int &ringDensity) : mesh(mesh), ringDensity(ringDensity) {
	this->textureXStep = 1.0f / (ringDensity - 1);
};

void LindenmayerTreeMeshGenerator::enqueueGenerationData(float &radius, glm::mat4 &transform, int textureY) {
	float theta = 2 * glm::pi<float>() / float(ringDensity - 1);
	int verticesOffset = mesh->vertices.size();

	for (int j = 0; j < ringDensity; j++) {
		mesh->vertices.push_back(Vertex());
		threadGenerationData.push_back(VertexGenerationAttributes(verticesOffset, j, theta, radius, transform, textureY));
	}
}

void LindenmayerTreeMeshGenerator::createIndiciesForSegment() {
	int bottomOffset = getVerticesLength() - 2 * getVerticesPerRing();
	int topOffset = bottomOffset + getVerticesPerRing();
	for (int i = 0; i < getVerticesPerRing() - 1; i++) {
		mesh->indices.push_back(bottomOffset + i);
		mesh->indices.push_back(topOffset + i);
		mesh->indices.push_back(bottomOffset + i + 1);

		mesh->indices.push_back(topOffset + i);
		mesh->indices.push_back(topOffset + i + 1);
		mesh->indices.push_back(bottomOffset + i + 1);
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
	glm::vec4 tmpPosition;
	glm::vec3 position, normal;
	Vertex *vert;
	VertexGenerationAttributes *vga;

	for (int i = startIndex; i < endIndex; i++) {
		vga = &threadGenerationData[i];
		arg = vga->theta*vga->vertexNumber;
		sin = sinf(arg) / 2;
		cos = cosf(arg) / 2;

		glm::mat3 rotationMatrix(vga->transform);

		tmpPosition = glm::vec4(vga->radius * cos, 0, vga->radius * sin, 1);
		position = glm::vec3(vga->transform * tmpPosition);
		normal = normalize(rotationMatrix * glm::vec3(tmpPosition) * glm::vec3(2, 2, 2));

		//output verticies
		vert = &mesh->vertices[vga->verticesOffset + vga->vertexNumber];
		vert->position = position;
		vert->normal = normal;
		vert->texCoords = glm::vec2(textureXStep * vga->vertexNumber, vga->textureY);
	}
}