#ifndef HDR_FRAME_BUFFER_H
#define HDR_FRAME_BUFFER_H

#include "FrameBuffer.h"
#include "Context.h"

class HDRFrameBuffer : public FrameBuffer {
public:
	HDRFrameBuffer();

	void executeFrameBuffer(Renderer& renderer);

protected:
	void init();
};

#endif