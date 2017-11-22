#include "Camera.h"

Camera::~Camera() {

}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(this->position, this->position + this->front, this->up);
}

glm::mat4 Camera::getProjectionMatrix() {
	return glm::perspective(ZOOM, float(Screen::getScreenWidth())/Screen::getScreenHeight(), 0.1f, 100.0f);
}

void Camera::processKeyboard(CameraDirectory direction, GLfloat deltaTime)
{
	GLfloat velocity = this->movementSpeed * deltaTime;
	if (direction == FORWARD)
		this->position += this->front * velocity;
	if (direction == BACKWARD)
		this->position -= this->front * velocity;
	if (direction == LEFT)
		this->position -= this->right * velocity;
	if (direction == RIGHT)
		this->position += this->right * velocity;
}


void Camera::processMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
	xoffset *= this->mouseSensitivity;
	yoffset *= this->mouseSensitivity;

	this->yaw += xoffset;
	this->pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (this->pitch > 89.0f)
			this->pitch = 89.0f;
		if (this->pitch < -89.0f)
			this->pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Eular angles
	this->updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::processMouseScroll(GLfloat yoffset)
{
	if (this->zoom >= 1.0f && this->zoom <= 45.0f)
		this->zoom -= yoffset;
	if (this->zoom <= 1.0f)
		this->zoom = 1.0f;
	if (this->zoom >= 45.0f)
		this->zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	front.y = sin(glm::radians(this->pitch));
	front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	this->front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	this->right = glm::normalize(glm::cross(this->front, this->worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	this->up = glm::normalize(glm::cross(this->right, this->front));
}

Ray Camera::cameraToViewportRay(Point2d mousePos) {
	//NDC dimensions
	mousePos.y = Screen::getScreenHeight() - mousePos.y;
	glm::vec4 lRayStart_NDC(
		(mousePos.x / Screen::getScreenWidth() - 0.5f) * 2.0f,
		(mousePos.y / Screen::getScreenHeight() - 0.5f) * 2.0f,
		-1.0,
		1.0
	);
	glm::vec4 lRayEnd_NDC(
		(mousePos.x / Screen::getScreenWidth() - 0.5f) * 2.0f,
		(mousePos.y / Screen::getScreenHeight() - 0.5f) * 2.0f,
		0.0,
		1.0f
	);

	mat4 InverseProjectionMatrix = inverse(getProjectionMatrix());

	// The View Matrix goes from World Space to Camera Space.
	// So inverse(ViewMatrix) goes from Camera Space to World Space.
	mat4 InverseViewMatrix = inverse(getViewMatrix());

	vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
	vec4 lRayStart_world = InverseViewMatrix		* lRayStart_camera; lRayStart_world /= lRayStart_world.w;
	vec4 lRayEnd_camera = InverseProjectionMatrix	* lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w;
	vec4 lRayEnd_world = InverseViewMatrix			* lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;

	//Directory vector
	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = glm::normalize(lRayDir_world);

	//Start point
	vec3 out_origin(lRayStart_world);
	//End point
	vec3 out_end = out_origin + lRayDir_world*1000.0f;

	Ray ray;
	ray.origin = out_origin;
	ray.end = out_end;
	ray.dir = lRayDir_world;

	return ray;
}