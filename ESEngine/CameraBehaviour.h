#ifndef CAMERABEHAVIOUR_H
#define CAMERABEHAVIOUR_H

#include "Behaviour.h"
#include "Camera.h"

class CameraBehaviour : public Behaviour {
public:
	CameraBehaviour(Camera *camera) : camera(camera), Behaviour() { };
	void update(double dt, InputState &inputState);

private:
	Camera *camera;
	double lastXPos = NULL;
	double lastYPos = NULL;

	void processMouseMove(InputState &inputState);
	void processMouseScroll(InputState &inputState);
	void processKeypad(double dt, InputState &inputState);
};

#endif