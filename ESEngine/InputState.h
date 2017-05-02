#ifndef INPUTSTATE_H
#define INPUTSTATE_H

#include <queue>
#include "Point2d.h"

using namespace std;

class InputState {
public:
	bool isKeyPressed(int key);
	bool isKeyReleased(int key);
	
	bool mousePositionQueueEmpty();
	bool mouseScrollQueueEmpty();
	Point2d popMousePosition();
	Point2d popMouseScroll();
	Point2d peekMousePosition();
	Point2d peekMouseScroll();

	void setKeyPressed(int key);
	void setKeyReleased(int key);
	void pushMousePosition(double x, double y);
	void pushMouseScroll(double x, double y);

private:
	bool keys[1024];
	queue<Point2d> mousePosition;
	queue<Point2d> mouseScroll;

};

#endif //!INPUTSTATE_H