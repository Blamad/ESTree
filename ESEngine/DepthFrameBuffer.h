#ifndef DEPTH_FRAME_BUFFER_H
#define DEPTH_FRAME_BUFFER_H

#include "FrameBuffer.h"

class DepthFrameBuffer : public FrameBuffer {
public:
	DepthFrameBuffer(unique_ptr<Shader> shader, int width, int height);

	void executeFrameBuffer(Renderer& renderer) { }

protected:
	void init();
};

#endif