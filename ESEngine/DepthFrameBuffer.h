#ifndef DEPTH_FRAME_BUFFER_H
#define DEPTH_FRAME_BUFFER_H

#include "FrameBuffer.h"

class DepthFrameBuffer : public FrameBuffer {
public:
	DepthFrameBuffer(unique_ptr<Shader> shader, int width, int height);

	void mountFrameBuffer();
	void unmountFrameBuffer();
	void executeFrameBuffer(Renderer& renderer);

	static void drawDepthFrame();
	
protected:
	unique_ptr<Shader> depthDebugShader;
	void init();

	//GUI DEPTH PREVIEW
	void initDebugQuad();

	static GLuint debugQuadVAO;
	static Shader* debugQuadShader;
	static TextureBuffer* debugQuadTextureBuffer;
};

#endif