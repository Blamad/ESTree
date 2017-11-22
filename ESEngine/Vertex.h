#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include <bitset>

using namespace glm;

enum VertexFlag {
	POSITION, NORMAL, TEXCOORDS, COLOR
};

struct Vertex {

	vec3 position;
	vec3 normal;
	vec2 texCoords;
	vec4 color;

	Vertex copy() {
		Vertex v;
		v.position = position;
		v.normal = normal;
		v.texCoords = texCoords;
		v.color = color;

		return v;
	}

	void transform(mat4 modelMatrix) {
		vec4 tmpPosition = vec4(position, 1);
		position = vec3(modelMatrix * tmpPosition);
		tmpPosition.z = 0;
		normal = normalize(vec3(modelMatrix * tmpPosition));
	}

	static Vertex createVertex(vec3 position, vec3 normal, vec2 texCoords) {
		Vertex vert;
		vert.position = position;
		vert.normal = normal;
		vert.texCoords = texCoords;

		return vert;
	}
};

#endif
