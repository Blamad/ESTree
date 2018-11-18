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

const vector<Point2d> InputState::getMousePositionEvents() {
	return mousePosition;
}

const Point2d InputState::getLastMousePosition() {
	return lastMousePosition;
}

const vector<Point2d> InputState::getMouseScrollEvents() {
	return mouseScroll;
}

const vector<ClickEvent> InputState::getMouseClickEvents() {
	return mouseClick;
}

const vector<KeyEvent> InputState::getKeyboardEvents() {
	return keyEvents;
}

const vector<char> InputState::getCharacterEvents() {
	return charEvents;
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
	charEvents.clear();
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

void InputState::pushCharEvent(char character) {
	charEvents.push_back(character);
}

void InputState::pushKeyEvent(int key, KeyState state) {
	keyEvents.push_back(KeyEvent(key, state));
	keys[key] = state == RELEASED ? false : true;
}

InputState::~InputState() {
	delete &mousePosition;
	delete &mouseScroll;
	delete &mouseClick;
	delete &keyEvents;
	delete &charEvents;
}