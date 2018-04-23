#include "InputState.h"

bool InputState::isKeyPressed(int key) {
	if (blocked)
		return false;
	return keys[key];
}

bool InputState::isKeyReleased(int key) {
	if (blocked)
		return false;
	return !keys[key];
}

const deque<Point2d> InputState::getMousePositionEvents() {
	return mousePosition;
}

const Point2d InputState::getLastMousePosition() {
	return lastMousePosition;
}

const deque<Point2d> InputState::getMouseScrollEvents() {
	return mouseScroll;
}

const deque<ClickEvent> InputState::getMouseClickEvents() {
	return mouseClick;
}

const deque<KeyEvent> InputState::getKeyboardEvents() {
	return keyEvents;
}

void InputState::blockEvents() {
	mousePosition.clear();
	mouseScroll.clear();
	mouseClick.clear();
	blocked = true;
}

void InputState::clearEvents() {
	mousePosition.clear();
	mouseScroll.clear();
	mouseClick.clear();
	keyEvents.clear();
	blocked = false;
}

void InputState::pushMousePosition(double x, double y) {
	mousePosition.push_back(Point2d(x, y));
	lastMousePosition = Point2d(x, y);
}

void InputState::pushMouseScroll(double x, double y) {
	mouseScroll.push_back(Point2d(x, y));
}

void InputState::pushMouseClick(ClickEvent click) {
	mouseClick.push_back(click);
}

void InputState::pushKeyEvent(int key, KeyState state) {
	keyEvents.push_back(KeyEvent(key, state));
	keys[key] = state == RELEASED ? false : true;
}