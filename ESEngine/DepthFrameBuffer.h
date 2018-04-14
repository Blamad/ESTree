#ifndef DEPTH_FRAME_BUFFER_H
#define DEPTH_FRAME_BUFFER_H

#include "FrameBuffer.h"

class DepthFrameBuffer : public FrameBuffer {
public:
	DepthFrameBuffer(shared_ptr<Shader> shader, int width, int height);

	void mountFrameBuffer();
	void unmountFrameBuffer();
	void executeFrameBuffer(Renderer& renderer) { };

	static TextureBuffer* getCurrentFrameTextureBuffer();
	
protected:
	shared_ptr<Shader> depthDebugShader;
	void init();
	static TextureBuffer* debugQuadTextureBuffer;
};

#endif