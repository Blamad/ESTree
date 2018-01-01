#include "DepthFrameBuffer.h"

Shader* DepthFrameBuffer::debugQuadShader = nullptr;
TextureBuffer* DepthFrameBuffer::debugQuadTextureBuffer = nullptr;
GLuint DepthFrameBuffer::debugQuadVAO = -1;

DepthFrameBuffer::DepthFrameBuffer(shared_ptr<Shader> shader, int width, int height) : FrameBuffer(shader, width, height) {
	depthDebugShader = ShaderManager::getInstance().getShader("Shaders/DepthQuadDebug.vert", "Shaders/DepthQuadDebug.frag");
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

	initDebugQuad();
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

void DepthFrameBuffer::executeFrameBuffer(Renderer& renderer) {
	depthDebugShader->use();
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, buffer->id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

//GUI PREVIEW STUFF

void DepthFrameBuffer::drawDepthFrame() {
	debugQuadShader->use();
	glBindVertexArray(debugQuadVAO);
	glBindTexture(GL_TEXTURE_2D, debugQuadTextureBuffer->id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DepthFrameBuffer::initDebugQuad() {
	debugQuadShader = depthDebugShader.get();
	debugQuadTextureBuffer = buffer.get();

	float bottomValue = -.9f;
	float topValue = -.6f;

	float quadVertices[] = {
		bottomValue,  topValue,  0.0f, 1.0f,
		bottomValue, bottomValue,  0.0f, 0.0f,
		topValue, bottomValue,  1.0f, 0.0f,

		bottomValue, topValue,  0.0f, 1.0f,
		topValue, bottomValue,  1.0f, 0.0f,
		topValue,  topValue,  1.0f, 1.0f
	};

	unsigned int quadVBO;
	glGenVertexArrays(1, &debugQuadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(debugQuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}