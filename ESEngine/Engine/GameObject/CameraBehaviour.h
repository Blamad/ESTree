#ifndef CAMERABEHAVIOUR_H
#define CAMERABEHAVIOUR_H

#include "Behaviour.h"
#include "Camera.h"
#include "Engine/Core/Logger.h"

class CameraBehaviour : public Behaviour {
public:
	CameraBehaviour(Camera *camera) : camera(camera), Behaviour() { };
	void update(double dt, InputState &inputState);
	void lockCameraRotation(bool lock);
	void clearCameraCache();

protected:
	static Logger logger;

private:
	Camera *camera;
	double lastXPos = NULL;
	double lastYPos = NULL;
	bool cursorVisible = true;
	bool cameraRotationLock = false;

	void processMouseMove(InputState &inputState);
	void processMouseScroll(InputState &inputState);
	void processKeypad(double dt, InputState &inputState);
};

#endif