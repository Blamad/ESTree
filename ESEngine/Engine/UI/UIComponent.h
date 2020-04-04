#ifndef UI_COMPONENT_H
#define UI_COMPONENT_H

#include "GraphicEngine/Input/InputState.h"

class UIComponent {
public:
	virtual void init() = 0;
	virtual void draw() = 0;
	virtual void update(double &dt, InputState &inputState) = 0;
};

#endif