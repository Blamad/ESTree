#ifndef INSTANCED_TRANSFORM_H
#define INSTANCED_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct InstancedTransform {
public:
	glm::mat4 modelMatrix = glm::mat4(1);
	//mat4 normalModelMatrix;

	void translateModel(glm::vec3 &position) {
		modelMatrix = translate(modelMatrix, position);
	}

	void rotateModel(float &angle, glm::vec3 &axis) {
		modelMatrix = rotate(modelMatrix, angle, axis);
	}

	void generateNormalModelMatrix() {
		//normalModelMatrix = transpose(inverse(modelMatrix));
	}
};

#endif
