#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include <bitset>

enum VertexFlag {
	POSITION, NORMAL, TEXCOORDS, COLOR
};

struct Vertex {

	std::bitset<4> type;
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec4 color;

};

#endif
