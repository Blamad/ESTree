#include "MouseManager.h"
#include <GLFW/glfw3.h>

Logger MouseManager::logger("MouseManager");

void MouseManager::update(double dt, InputState &inputState) {
	processTriggeringInputEvents(inputState);
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
	BOOST_FOREACH (Point2d const& mousePos, inputState.getMousePositionEvents()) {
		if (lastXPos != NULL && lastYPos != NULL)
			moveGameObject(pickedGameObject, mousePos.x - lastXPos, mousePos.y - lastYPos);
		lastXPos = mousePos.x;
		lastYPos = mousePos.y;
	}
}

void MouseManager::pickObject(GameObject* go) {	
	RigidBody *rb = (RigidBody*)go->getComponent(RIGIDBODY);
	if (!rb->pickable) {
		return;
	}

	pickedGameObject = go;
	pickedGameObjectMass = rb->getCurrentMass();
	rb->changeMass(0);
	rb->clearForces();
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

bool MouseManager::checkIfMouseRightClickEventHappened(InputState& inputState, bool pressed) {
	BOOST_FOREACH(ClickEvent const& click, inputState.getMouseClickEvents()) {
		if (click.button == MOUSE_BUTTON_RIGHT && click.pressed == pressed)
			return true;
	}
	return false;
}

void MouseManager::processTriggeringInputEvents(InputState &inputState) {
	//Enter camera control mode
	if (checkIfMouseRightClickEventHappened(inputState, true) && objectPickingMode) {
		objectPickingMode = false;
		if (pickedGameObject != nullptr)
			releaseObject();
		Screen::setCursorState(DISABLED);

		//Avoid camera jumping
		cameraBehaviour->clearCameraCache();
	}

	//Enter object picking mode
	if (checkIfMouseRightClickEventHappened(inputState, false) && !objectPickingMode) {
		objectPickingMode = true;
		Screen::setCursorState(VISIBLE);
	}
}