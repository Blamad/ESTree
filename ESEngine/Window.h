#ifndef WINDOW_H
#define WINDOW_H

#include <memory>
#include "Renderer.h"
#include "InputState.h"

using namespace std;

class Window {
public:
	Window(int width, int height) : width(width), height(height) { aspectRatio = width / height; };

	virtual bool initialize() = 0;
	virtual bool shouldClose() = 0;
	virtual void poolInputEvents() = 0;
	virtual void prepareFrameRendering() = 0;
	virtual void finishFrameRendering() = 0;

	float getAspectRatio() { return aspectRatio; };

	virtual double getTime() = 0;
	virtual InputState* registerInputManager() = 0;

protected:
	int width, height;
	float aspectRatio;
private:
	Window();
};

#endif // !WINDOW_H
