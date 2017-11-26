#include "DepthFrameBuffer.h"

DepthFrameBuffer::DepthFrameBuffer(unique_ptr<Shader> shader, int width, int height) : FrameBuffer(move(shader), width, height) {
	init();
};

void DepthFrameBuffer::init() {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//Depth buffer
	buffer = unique_ptr<TextureBuffer>(new TextureBuffer());
	buffer->setTextureBuffer(nullptr, width, height, 3, DEPTH_BUFFER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer->id, 0);

	//No need for stencil/color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}