#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include "Component.h"
#include "GraphicEngine/Input/InputState.h"

class Behaviour : public Component {
public:
	Behaviour() : Component(BEHAVIOUR) {};

	virtual void update(double dt, InputState &inputState) = 0;
};

#endif