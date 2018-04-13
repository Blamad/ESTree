#include "DepthFramePreviewComponent.h"

void DepthFramePreviewComponent::draw() {
	if (!initialized)
		init();
	depthDebugShader->use();
	glBindVertexArray(debugQuadVAO);
	glBindTexture(GL_TEXTURE_2D, DepthFrameBuffer::getCurrentFrameTextureBuffer()->id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DepthFramePreviewComponent::init() {
	depthDebugShader = unique_ptr<Shader>(new Shader("Shaders/DepthQuadDebug.vert", "Shaders/DepthQuadDebug.frag"));

	float quadVertices[] = {
		topLeftDebugQuadCorner.x - debugQuadSize, topLeftDebugQuadCorner.y,					0.0f, 1.0f,
		topLeftDebugQuadCorner.x - debugQuadSize, topLeftDebugQuadCorner.y - debugQuadSize, 0.0f, 0.0f,
		topLeftDebugQuadCorner.x, topLeftDebugQuadCorner.y - debugQuadSize,					1.0f, 0.0f,

		topLeftDebugQuadCorner.x - debugQuadSize, topLeftDebugQuadCorner.y,					0.0f, 1.0f,
		topLeftDebugQuadCorner.x, topLeftDebugQuadCorner.y - debugQuadSize,					1.0f, 0.0f,
		topLeftDebugQuadCorner.x, topLeftDebugQuadCorner.y,									1.0f, 1.0f
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