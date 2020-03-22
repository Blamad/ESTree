#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include <bitset>

enum VertexFlag {
	POSITION, NORMAL, TEXCOORDS, COLOR
};

struct Vertex {

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec4 color;

	Vertex copy() {
		Vertex v;
		v.position = position;
		v.normal = normal;
		v.texCoords = texCoords;
		v.color = color;

		return v;
	}

	void transform(glm::mat4 modelMatrix) {
		glm::vec4 tmpPosition = glm::vec4(position, 1);
		position = glm::vec3(modelMatrix * tmpPosition);
		tmpPosition.z = 0;
		normal = normalize(glm::vec3(modelMatrix * tmpPosition));
	}

	static Vertex createVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords) {
		Vertex vert;
		vert.position = position;
		vert.normal = normal;
		vert.texCoords = texCoords;

		return vert;
	}
};

#endif
