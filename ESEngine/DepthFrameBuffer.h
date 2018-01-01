#ifndef DEPTH_FRAME_BUFFER_H
#define DEPTH_FRAME_BUFFER_H

#include "FrameBuffer.h"

class DepthFrameBuffer : public FrameBuffer {
public:
	DepthFrameBuffer(shared_ptr<Shader> shader, int width, int height);

	void mountFrameBuffer();
	void unmountFrameBuffer();
	void executeFrameBuffer(Renderer& renderer);

	static void drawDepthFrame();
	
protected:
	shared_ptr<Shader> depthDebugShader;
	void init();

	//GUI DEPTH PREVIEW
	void initDebugQuad();

	static GLuint debugQuadVAO;
	static Shader* debugQuadShader;
	static TextureBuffer* debugQuadTextureBuffer;
};

#endif