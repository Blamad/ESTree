#ifndef CLICKEVENT_H
#define CLICKEVENT_H

#include "Point2d.h"

enum MouseButton {
	MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT, MOUSE_BUTTON_MIDDLE
};

enum MouseModifier {
	MOUSE_MOD_NONE, MOUSE_MOD_SHIFT, MOUSE_MOD_CONTROL, MOUSE_MOD_ALT
};

class ClickEvent {
public:
	ClickEvent(MouseButton button, bool pressed, Point2d position, MouseModifier modifier = MOUSE_MOD_NONE) : button(button), pressed(pressed), position(position), modifier(modifier) {};
	MouseButton button;
	bool pressed;
	MouseModifier modifier;
	Point2d position;

private:
	ClickEvent() {};
};

#endif