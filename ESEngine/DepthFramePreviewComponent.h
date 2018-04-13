#ifndef DEPTH_FRAME_PREVIEW_COMPONENT_H
#define DEPTH_FRAME_PREVIEW_COMPONENT_H

#include "UIComponent.h"
#include "DepthFrameBuffer.h"
#include "Shader.h"

class DepthFramePreviewComponent : public UIComponent {
public:
	DepthFramePreviewComponent(vec2 topLeftDebugQuadCorner = vec2(-.6f, -.6f), float debugQuadSize = .3f) : topLeftDebugQuadCorner(topLeftDebugQuadCorner), debugQuadSize(debugQuadSize), initialized(false) { }

	void init();
	void draw();
	void update(double &dt, InputState &inputState) { };

private:
	bool initialized;
	vec2 topLeftDebugQuadCorner;
	float debugQuadSize;
	unique_ptr<Shader> depthDebugShader;

	GLuint debugQuadVAO;
};

#endif