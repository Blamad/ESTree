#include "InputState.h"

bool InputState::isKeyPressed(int key) {
	return keys[key];
}

bool InputState::isKeyReleased(int key) {
	return keys[key];
}

Point2d InputState::peekMousePosition() {
	return mousePosition.back();
}

Point2d InputState::popMousePosition() {
	Point2d element = peekMousePosition();
	mousePosition.pop();
	return element;
}

Point2d InputState::peekMouseScroll() {
	return mouseScroll.back();
}

Point2d InputState::popMouseScroll() {
	Point2d element = peekMouseScroll();
	mousePosition.pop();
	return element;
}

void InputState::setKeyPressed(int key) {
	keys[key] = true;
}

void InputState::setKeyReleased(int key) {
	keys[key] = false;
}

void InputState::pushMousePosition(double x, double y) {
	mousePosition.push(Point2d(x, y));
}

void InputState::pushMouseScroll(double x, double y) {
	mouseScroll.push(Point2d(x, y));
}