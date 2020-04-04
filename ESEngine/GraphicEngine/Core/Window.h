#ifndef WINDOW_H
#define WINDOW_H

#include <memory>
#include "Renderer.h"
#include "GraphicEngine/Input/InputState.h"

class Window {
public:
	Window(int width, int height, bool fullScreenMode) : width(width), height(height), fullScreenMode(fullScreenMode) {
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
	bool isFullScreenMode() { return fullScreenMode; }
	float getAspectRatio() { return aspectRatio; };

	virtual double getTime() = 0;
	virtual InputState* registerInputManager() = 0;

protected:
	float aspectRatio;
	int width, height;
	bool fullScreenMode;
private:
	Window();
};

#endif // !WINDOW_H
