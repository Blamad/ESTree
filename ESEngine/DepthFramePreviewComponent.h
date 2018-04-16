#ifndef DEPTH_FRAME_PREVIEW_COMPONENT_H
#define DEPTH_FRAME_PREVIEW_COMPONENT_H

#include "UIComponent.h"
#include "DepthFrameBuffer.h"
#include "ShaderManager.h"
#include "Screen.h"

class DepthFramePreviewComponent : public UIComponent {
public:
	DepthFramePreviewComponent(GLfloat xpos = Screen::getScreenWidth() * 0.8, GLfloat ypos = Screen::getScreenHeight() * 0.0, GLfloat width = Screen::getScreenWidth() * 0.2, GLfloat height = Screen::getScreenHeight() * 0.2) : xpos(xpos), ypos(ypos), width(width), height(height), initialized(false), enabled(true) { }

	bool enabled;

	void init();
	void draw();
	void update(double &dt, InputState &inputState) { };

private:
	GLfloat xpos;
	GLfloat ypos;
	GLfloat height;
	GLfloat width;
	bool initialized;
	shared_ptr<Shader> shader;

	GLuint VAO, VBO;
};

#endif