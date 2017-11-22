#include "MouseManager.h"
#include <GLFW/glfw3.h>

Logger MouseManager::logger("MouseManager");

void MouseManager::update(double dt, InputState &inputState) {
	processKeyboardShortcuts(inputState);
	if (objectPickingMode)
		updatePhysicsWorld(dt, inputState);
	else
		updateCameraBehaviour(dt, inputState);
}

void MouseManager::updatePhysicsWorld(double dt, InputState &inputState) {
	for (ClickEvent const& click : inputState.getMouseClickEvents())
		processMouseClick(click);

	if (pickedGameObject != nullptr) {
		updatePickedObject(dt, inputState);
	}
}

void MouseManager::processMouseClick(ClickEvent const& click) {
	if (click.button == MOUSE_BUTTON_LEFT && click.pressed) {
		GameObject *go = raycastClickToGameObject(click.position);
		if (go != nullptr)
			pickObject(go);
	}

	if (click.button == MOUSE_BUTTON_LEFT && !click.pressed && pickedGameObject != nullptr)
		releaseObject();
}

GameObject* MouseManager::raycastClickToGameObject(Point2d mousePos) {
	return pManager->raycast(camera->cameraToViewportRay(mousePos));
}

void MouseManager::updatePickedObject(double dt, InputState& inputState) {
	for (Point2d const& mousePos : inputState.getMousePositionEvents()) {
		if (lastXPos != NULL && lastYPos != NULL)
			moveGameObject(pickedGameObject, mousePos.x - lastXPos, mousePos.y - lastYPos);
		lastXPos = mousePos.x;
		lastYPos = mousePos.y;
	}
}

void MouseManager::pickObject(GameObject* go) {	
	RigidBody *rb = (RigidBody*)go->getComponent(RIGIDBODY);
	if (!rb->pickable) {
		if (go->name == "Floor")
			rb->translate(vec3(0, -1, 0));
		return;
	}

	pickedGameObject = go;
	pickedGameObjectMass = rb->getCurrentMass();
	rb->changeMass(0);
	logger.log(INFO, "Rigidbody picked...");

	Screen::setCursorState(HIDDEN);
}

void MouseManager::releaseObject() {
	RigidBody *rb = (RigidBody*)pickedGameObject->getComponent(RIGIDBODY);
	rb->changeMass(pickedGameObjectMass);

	pickedGameObject = nullptr;
	lastXPos = NULL;
	lastYPos = NULL;
	logger.log(INFO, "Rigidbody released!");

	Screen::setCursorState(VISIBLE);
}

void MouseManager::moveGameObject(GameObject *go, double x, double y) {

	vec3 distVec = camera->position - ((Transform*)go->getComponent(TRANSFORM))->getPosition();
	float dist = sqrtf(dot(distVec, distVec));
	
	double distanceMod = dist * 0.0015;
	vec4 transform(x * distanceMod, -y * distanceMod, 0, 0);
	transform = transform * camera->getViewMatrix();
	
	((RigidBody*)go->getComponent(RIGIDBODY))->translate(vec3(transform));
}

float MouseManager::calcDistance(vec3 vecA, vec3 vecB) {
	vec3 tmp = vecA - vecB;
	return sqrtf((powf(tmp.x, 2) + powf(tmp.y, 2) + powf(tmp.z, 2)));
}

string MouseManager::vecToString(vec3 vec) {
	return "["+std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + "]";
}

void MouseManager::updateCameraBehaviour(double dt, InputState &inputState) {
	cameraBehaviour->update(dt, inputState);
}

void MouseManager::processKeyboardShortcuts(InputState &inputState) {
	//Enter camera control mode
	if ((inputState.isKeyPressed(GLFW_KEY_LEFT_CONTROL) || inputState.isKeyPressed(GLFW_KEY_RIGHT_CONTROL)) && objectPickingMode) {
		objectPickingMode = false;
		if (pickedGameObject != nullptr)
			releaseObject();
		Screen::setCursorState(DISABLED);

		//Avoid camera jumping
		cameraBehaviour->clearCameraCache();
	}

	//Enter object picking mode
	if ((inputState.isKeyReleased(GLFW_KEY_LEFT_CONTROL) && inputState.isKeyReleased(GLFW_KEY_RIGHT_CONTROL)) && !objectPickingMode) {
		objectPickingMode = true;
		Screen::setCursorState(VISIBLE);
	}
}