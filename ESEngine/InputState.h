#ifndef INPUTSTATE_H
#define INPUTSTATE_H

#include <vector>
#include "Point2d.h"
#include "ClickEvent.h"
#include "KeyEvent.h"

#include <boost\foreach.hpp>

using namespace std;

class InputState {
public:
	InputState() : blocked(false) {}
	~InputState();

	bool isKeyPressed(int key);
	bool isKeyReleased(int key);
	
	const vector<Point2d> InputState::getMousePositionEvents();
	const vector<Point2d> InputState::getMouseScrollEvents();
	const vector<ClickEvent> InputState::getMouseClickEvents();
	const vector<KeyEvent> InputState::getKeyboardEvents();
	const vector<char> InputState::getCharacterEvents();

	const Point2d getLastMousePosition();

	void blockEvents();
	void clearEvents();

	void setKeyPressed(int key);
	void setKeyReleased(int key);
	void pushMousePosition(double x, double y);
	void pushMouseScroll(double x, double y);
	void pushMouseClick(ClickEvent click);
	void pushKeyEvent(int key, KeyState state);
	void pushCharEvent(char character);

private:
	vector<Point2d> mousePosition;
	vector<Point2d> mouseScroll;
	vector<ClickEvent> mouseClick;
	vector<KeyEvent> keyEvents;
	vector<char> charEvents;
	bool keys[500];
	bool blocked;

	Point2d lastMousePosition;
};

#endif //!INPUTSTATE_H