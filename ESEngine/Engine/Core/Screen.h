#ifndef SCREEN_H
#define SCREEN_H

#include "GraphicEngine/Core/Window.h"

enum CursorState {
	VISIBLE, HIDDEN, DISABLED
};

class Screen {
public:
	static void setInstance(Window* window);

	static int getScreenWidth();
	static int getScreenHeight();
	static void setViewport(int width, int height);
	static void setCursorState(CursorState state);

private:
	static Window* instance;
};

#endif