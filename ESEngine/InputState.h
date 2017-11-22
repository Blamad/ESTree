#ifndef INPUTSTATE_H
#define INPUTSTATE_H

#include <deque>
#include "Point2d.h"
#include "ClickEvent.h"

using namespace std;

class InputState {
public:
	bool isKeyPressed(int key);
	bool isKeyReleased(int key);
	
	const deque<Point2d> InputState::getMousePositionEvents();
	const deque<Point2d> InputState::getMouseScrollEvents();
	const deque<ClickEvent> InputState::getMouseClickEvents();

	const Point2d getLastMousePosition();

	void clearMouseEvents();

	void setKeyPressed(int key);
	void setKeyReleased(int key);
	void pushMousePosition(double x, double y);
	void pushMouseScroll(double x, double y);
	void pushMouseClick(ClickEvent click);

private:
	bool keys[1024];
	deque<Point2d> mousePosition;
	deque<Point2d> mouseScroll;
	deque<ClickEvent> mouseClick;

	Point2d lastMousePosition;
};

#endif //!INPUTSTATE_H