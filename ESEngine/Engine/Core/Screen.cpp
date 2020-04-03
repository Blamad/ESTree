#include "Screen.h"

Window* Screen::instance = nullptr;

int Screen::getScreenWidth() {
	if (instance) {
		return instance->getWidth();
	}
}

int Screen::getScreenHeight() {
	if (instance) {
		return instance->getHeight();
	}
}

void Screen::setViewport(int width, int height) {
	if (instance) {
		instance->setViewport(width, height);
	}
}

void Screen::setCursorState(CursorState state) {
	switch (state) {
		case VISIBLE:
			instance->setCursorVisible();
			break;
		case HIDDEN:
			instance->setCursorHidden();
			break;
		case DISABLED:
			instance->setCursorDisabled();
			break;
	}
}

void Screen::setInstance(Window* window) {
	instance = window;
}