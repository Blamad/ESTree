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
};

#endif