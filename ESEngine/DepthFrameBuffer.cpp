#include "DepthFrameBuffer.h"

TextureBuffer* DepthFrameBuffer::debugQuadTextureBuffer = nullptr;


DepthFrameBuffer::DepthFrameBuffer(shared_ptr<Shader> shader, int width, int height) : FrameBuffer(move(shader), width, height) {
	depthDebugShader = ShaderManager::getShader("Shaders/DepthQuadDebug.vert", "Shaders/DepthQuadDebug.frag");
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

	//For UI preview
	debugQuadTextureBuffer = buffer.get();
}

void DepthFrameBuffer::mountFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Screen::setViewport(width, height);
}

void DepthFrameBuffer::unmountFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	Screen::setViewport(Screen::getScreenWidth(), Screen::getScreenHeight());
}

//GUI PREVIEW STUFF

TextureBuffer* DepthFrameBuffer::getCurrentFrameTextureBuffer() {
	return debugQuadTextureBuffer;
}