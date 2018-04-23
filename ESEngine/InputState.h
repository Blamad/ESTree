#ifndef INPUTSTATE_H
#define INPUTSTATE_H

#include <deque>
#include "Point2d.h"
#include "ClickEvent.h"
#include "KeyEvent.h"

#include <boost\foreach.hpp>

using namespace std;

class InputState {
public:
	InputState() : blocked(false) {}

	bool isKeyPressed(int key);
	bool isKeyReleased(int key);
	
	const deque<Point2d> InputState::getMousePositionEvents();
	const deque<Point2d> InputState::getMouseScrollEvents();
	const deque<ClickEvent> InputState::getMouseClickEvents();
	const deque<KeyEvent> InputState::getKeyboardEvents();

	const Point2d getLastMousePosition();

	void blockEvents();
	void clearEvents();

	void setKeyPressed(int key);
	void setKeyReleased(int key);
	void pushMousePosition(double x, double y);
	void pushMouseScroll(double x, double y);
	void pushMouseClick(ClickEvent click);
	void pushKeyEvent(int key, KeyState state);

private:
	deque<Point2d> mousePosition;
	deque<Point2d> mouseScroll;
	deque<ClickEvent> mouseClick;
	deque<KeyEvent> keyEvents;
	bool keys[500];
	bool blocked;

	Point2d lastMousePosition;
};

#endif //!INPUTSTATE_H