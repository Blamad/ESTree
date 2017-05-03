#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Component.h"

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;

enum CameraDirectory {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera : public Component {
public:
 
	Camera(glm::vec3 position, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : position(position), worldUp(up), yaw(yaw), pitch(pitch), Component(CAMERA), movementSpeed(SPEED), mouseSensitivity(SENSITIVTY), zoom(ZOOM) {
		this->updateCameraVectors();
	};

	~Camera();

	// Camera Attributes
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	// Eular Angles
	GLfloat yaw;
	GLfloat pitch;
	// Camera options
	GLfloat movementSpeed;
	GLfloat mouseSensitivity;
	GLfloat zoom;

	void processKeyboard(CameraDirectory direction, GLfloat deltaTime);
	void processMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
	void processMouseScroll(GLfloat yoffset);

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix(float aspectRatio);

private:
	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors();
};

#endif