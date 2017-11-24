#ifndef WINDOW_H
#define WINDOW_H

#include <memory>
#include "Renderer.h"
#include "InputState.h"

using namespace std;

class Window {
public:
	Window(int width, int height) : width(width), height(height) { 
		aspectRatio = width / height;
	};

	virtual bool initialize() = 0;
	virtual bool shouldClose() = 0;
	virtual void poolInputEvents() = 0;
	virtual void prepareFrameRendering() = 0;
	virtual void finishFrameRendering() = 0;

	virtual void setCursorVisible() = 0;
	virtual void setCursorHidden() = 0;
	virtual void setCursorDisabled() = 0;

	virtual void setViewport(int width, int height) = 0;

	int getWidth() { return width; }
	int getHeight() { return height; }
	float getAspectRatio() { return aspectRatio; };

	virtual double getTime() = 0;
	virtual InputState* registerInputManager() = 0;

protected:
	float aspectRatio;
	int width, height;
private:
	Window();
};

#endif // !WINDOW_H
