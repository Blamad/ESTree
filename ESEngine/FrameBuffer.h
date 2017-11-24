#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "Renderer.h"
#include "Screen.h"
#include "Texture.h"
#include "Logger.h"

class FrameBuffer {
public:
	GLuint FBO;
	unique_ptr<Shader> shader;

	FrameBuffer(unique_ptr<Shader> shader);

	void initAsDepthBuffer(int width, int height);
	void initAsColorBuffer();

	TextureBuffer* getDepthBuffer();
	TextureBuffer* getColorBuffer();

	void mountFrameBuffer();
	void unmountFrameBuffer();
	void executeFrameBuffer(Renderer& renderer);

private:
	static Logger logger;

	int width, height;
	unique_ptr<TextureBuffer> depthBuffer;
	unique_ptr<TextureBuffer> colorBuffer;
	GLuint quadVAO;
};

#endif