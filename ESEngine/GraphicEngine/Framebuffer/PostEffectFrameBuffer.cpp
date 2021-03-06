#include "PostEffectFrameBuffer.h"

PostEffectFrameBuffer::PostEffectFrameBuffer() : FrameBuffer() {
	this->width = Screen::getScreenWidth();
	this->height = Screen::getScreenHeight();
	this->shader = ShaderManager::getShader("Resources/Shaders/PostEffectShader.vert", "Resources/Shaders/PostEffectShader.frag");
	init();
};

void PostEffectFrameBuffer::init() {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//Depth and stencil buffer in rbo since there is no read-operation needed
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//Color buffer
	buffer = std::unique_ptr<TextureBuffer>(new TextureBuffer());
	buffer->setTextureBuffer(nullptr, width, height, 3, BUFFER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->id, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	initDrawingQuad();
}

void PostEffectFrameBuffer::executeFrameBuffer(Renderer& renderer) {
	shader->use();
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, buffer->id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}