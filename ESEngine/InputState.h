#ifndef INPUTSTATE_H
#define INPUTSTATE_H

#include <deque>
#include "Point2d.h"

using namespace std;

class InputState {
public:
	bool isKeyPressed(int key);
	bool isKeyReleased(int key);
	
	const deque<Point2d> InputState::getMousePositionEvents();
	const deque<Point2d> InputState::getMouseScrollEvents();

	void clearMouseEvents();

	void setKeyPressed(int key);
	void setKeyReleased(int key);
	void pushMousePosition(double x, double y);
	void pushMouseScroll(double x, double y);

private:
	bool keys[1024];
	deque<Point2d> mousePosition;
	deque<Point2d> mouseScroll;

};

#endif //!INPUTSTATE_H