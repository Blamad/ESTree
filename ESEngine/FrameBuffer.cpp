#include "FrameBuffer.h"

Logger FrameBuffer::logger("FrameBuffer");

FrameBuffer::FrameBuffer(unique_ptr<Shader> shader) {
	this->shader = move(shader);
};

TextureBuffer* FrameBuffer::getDepthBuffer() {
	return depthBuffer.get();
}

TextureBuffer* FrameBuffer::getColorBuffer() {
	return depthBuffer.get();
}

void FrameBuffer::initAsDepthBuffer(int width, int height) {
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//Depth buffer
	depthBuffer = unique_ptr<TextureBuffer>(new TextureBuffer());
	depthBuffer->setTextureBuffer(nullptr, width, height, 3, DEPTH_BUFFER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer->id, 0);

	//No need for stencil/color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::initAsColorBuffer() {
	this->width = Screen::getScreenWidth();
	this->height = Screen::getScreenHeight();

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//Depth and stencil buffer in rbo since there is no read-operation needed
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//Color buffer
	colorBuffer = unique_ptr<TextureBuffer>(new TextureBuffer());
	colorBuffer->setTextureBuffer(nullptr, width, height, 3, BUFFER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer->id, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float quadVertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};
	unsigned int quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void FrameBuffer::mountFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Screen::setViewport(width, height);
}

void FrameBuffer::unmountFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	Screen::setViewport(Screen::getScreenWidth(), Screen::getScreenHeight());
}

void FrameBuffer::executeFrameBuffer(Renderer& renderer) {
	if (colorBuffer != nullptr) {
		shader->use();
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, colorBuffer->id);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	if (depthBuffer != nullptr) {
		//Do nothing, depth buffer only stores data in it's buffer
	}
}