#ifndef MOUSE_MANAGER_H
#define MOUSE_MANAGER_H

#include <glm/glm.hpp>
#include "PhysicsManager.h"
#include "CameraBehaviour.h"
#include "Camera.h"
#include "InputState.h"

#include <cmath>

class MouseManager {
public:
	MouseManager(PhysicsManager *pManager, CameraBehaviour *cameraBehaviour, Camera *camera) : pManager(pManager), cameraBehaviour(cameraBehaviour), camera(camera), objectPickingMode(true) { };
	MouseManager(PhysicsManager *pManager) : pManager(pManager), objectPickingMode(true) { };

	void setCamera(CameraBehaviour *cameraBehaviour, Camera *camera) { 
		this->camera = camera; 
		this->cameraBehaviour = cameraBehaviour;
	};
	
	void update(double dt, InputState &inputState);

private:
	static Logger logger;
	
	PhysicsManager *pManager;
	Camera *camera;
	CameraBehaviour *cameraBehaviour;
	bool objectPickingMode;

	void processKeyboardShortcuts(InputState & inputState);
	void updateCameraBehaviour(double dt, InputState & inputState);
	void updatePhysicsWorld(double dt, InputState & inputState);

	//RigidBody stuff

	GameObject *pickedGameObject;
	double pickedGameObjectMass = -1;
	double lastXPos = NULL;
	double lastYPos = NULL;

	GameObject* raycastClickToGameObject(Point2d mousePos);
	void processMouseClick(ClickEvent const& ce);
	void updatePickedObject(double dt, InputState& inputState);
	void moveGameObject(GameObject *go, double x, double y);
	void pickObject(GameObject * go);
	void releaseObject();

	string vecToString(vec3 vec); 
	float calcDistance(vec3 vecA, vec3 vecB);
};

#endif