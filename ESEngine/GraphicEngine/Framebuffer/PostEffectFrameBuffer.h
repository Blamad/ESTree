#ifndef POST_EFFECT_FRAME_BUFFER_H
#define POST_EFFECT_FRAME_BUFFER_H

#include "FrameBuffer.h"

class PostEffectFrameBuffer : public FrameBuffer {
public:
	PostEffectFrameBuffer();

	void executeFrameBuffer(Renderer& renderer);

protected:
	void init();
};

#endif