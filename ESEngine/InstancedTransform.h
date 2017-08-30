#ifndef INSTANCED_TRANSFORM_H
#define INSTANCED_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

struct InstancedTransform {
public:
	mat4 modelMatrix = mat4(1);
	//mat4 normalModelMatrix = mat4(1);

	void translateModel(vec3 &position) {
		modelMatrix = translate(modelMatrix, position);
	}

	void rotateModel(float &angle, vec3 &axis) {
		modelMatrix = rotate(modelMatrix, angle, axis);
	}

	void generateNormalModelMatrix() {
		//normalModelMatrix = transpose(inverse(modelMatrix));
	}
};

#endif
