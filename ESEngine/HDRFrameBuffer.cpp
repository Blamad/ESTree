#include "HDRFrameBuffer.h"

HDRFrameBuffer::HDRFrameBuffer() : FrameBuffer() {
	this->width = Screen::getScreenWidth() * 2;
	this->height = Screen::getScreenHeight() * 2;
	this->shader = ShaderManager::getShader("Resources/Shaders/HDRShader.vert", "Resources/Shaders/HDRShader.frag");
	init();
};

void HDRFrameBuffer::init() {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//Depth and stencil buffer in rbo since there is no read-operation needed
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//HDR buffer
	buffer = std::unique_ptr<TextureBuffer>(new TextureBuffer());
	buffer->setTextureBuffer(nullptr, width, height, 0, FLOATING_POINT_COLOR_BUFFER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->id, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	initDrawingQuad();

	shader->registerSubroutine("noHDR", GL_FRAGMENT_SHADER);
	shader->registerSubroutine("adaptativeToneMapping", GL_FRAGMENT_SHADER);
}

void HDRFrameBuffer::executeFrameBuffer(Renderer& renderer) {
	shader->use();
	Context::getHdrToggle() ? shader->setShaderSubroutine("adaptativeToneMapping") : shader->setShaderSubroutine("noHDR");
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, buffer->id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}