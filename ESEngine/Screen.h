#ifndef SCREEN_H
#define SCREEN_H

#include "Window.h"

enum CursorState {
	VISIBLE, HIDDEN, DISABLED
};

class Screen {
public:
	static void setInstance(Window* window);

	static int getScreenWidth();
	static int getScreenHeight();
	static void setCursorState(CursorState state);

private:
	static Window* instance;
};

#endif