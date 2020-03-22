#ifndef DEPTH_FRAME_BUFFER_H
#define DEPTH_FRAME_BUFFER_H

#include "FrameBuffer.h"

class DepthFrameBuffer : public FrameBuffer {
public:
	DepthFrameBuffer(std::shared_ptr<Shader> shader, int width, int height);

	void mountFrameBuffer();
	void unmountFrameBuffer();
	void executeFrameBuffer(Renderer& renderer) { };

	static TextureBuffer* getCurrentFrameTextureBuffer();
	
protected:
	void init();
	static TextureBuffer* debugQuadTextureBuffer;
};

#endif