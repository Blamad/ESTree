#ifndef MOUSE_MANAGER_H
#define MOUSE_MANAGER_H

#include <glm/glm.hpp>
#include <boost/foreach.hpp>
#include "PhysicsManager.h"
#include "CameraBehaviour.h"
#include "Camera.h"
#include "InputState.h"
#include "Context.h"

#include <cmath>

class MouseManager {
public:
	MouseManager(PhysicsManager *pManager, CameraBehaviour *cameraBehaviour, Camera *camera) : pManager(pManager), cameraBehaviour(cameraBehaviour), camera(camera), objectPickingMode(true) {
		Context::setMouseManager(this);
	};
	MouseManager(PhysicsManager *pManager) : pManager(pManager), objectPickingMode(true) {
		Context::setMouseManager(this);
	};

	void setCamera(CameraBehaviour *cameraBehaviour, Camera *camera) { 
		this->camera = camera; 
		this->cameraBehaviour = cameraBehaviour;
	};
	
	void update(double dt, InputState &inputState);

	GameObject* getSelectedGameObject() {
		return selectedGameObject;
	}

	void setSelectedGameObject(GameObject *go) {
		selectedGameObject = go;
	}

private:
	static Logger logger;
	
	PhysicsManager *pManager = nullptr;
	Camera *camera = nullptr;
	CameraBehaviour *cameraBehaviour = nullptr;
	bool objectPickingMode;

	void processTriggeringInputEvents(InputState & inputState);
	void updateCameraBehaviour(double dt, InputState & inputState);
	void updatePhysicsWorld(double dt, InputState & inputState);

	bool checkIfMouseRightClickEventHappened(InputState& inputState, bool pressed);
	//RigidBody stuff

	GameObject *pickedGameObject = nullptr;
	GameObject *selectedGameObject = nullptr;
	double pickedGameObjectMass = -1;
	double lastXPos = NULL;
	double lastYPos = NULL;

	GameObject* raycastClickToGameObject(Point2d mousePos);
	void processMouseClick(ClickEvent const& ce);
	void updatePickedObject(double dt, InputState& inputState);
	void moveGameObject(GameObject *go, double x, double y);
	void pickObject(GameObject * go);
	void releaseObject();

	std::string vecToString(glm::vec3 vec);
	float calcDistance(glm::vec3 vecA, glm::vec3 vecB);
};

#endif