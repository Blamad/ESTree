#include "InputState.h"

bool InputState::isKeyPressed(int key) {
	return keys[key];
}

bool InputState::isKeyReleased(int key) {
	return keys[key];
}

const deque<Point2d> InputState::getMousePositionEvents() {
	return mousePosition;
}

const deque<Point2d> InputState::getMouseScrollEvents() {
	return mouseScroll;
}

void InputState::clearMouseEvents() {
	mousePosition.clear();
	mouseScroll.clear();
}

void InputState::setKeyPressed(int key) {
	keys[key] = true;
}

void InputState::setKeyReleased(int key) {
	keys[key] = false;
}

void InputState::pushMousePosition(double x, double y) {
	mousePosition.push_back(Point2d(x, y));
}

void InputState::pushMouseScroll(double x, double y) {
	mouseScroll.push_back(Point2d(x, y));
}