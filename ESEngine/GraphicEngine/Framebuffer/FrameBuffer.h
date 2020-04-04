#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "Engine/Manager/ShaderManager.h"
#include "GraphicEngine/Core/Renderer.h"
#include "Engine/Core/Screen.h"
#include "Engine/Core/Texture.h"
#include "Engine/Core/Logger.h"

class FrameBuffer {
public:
	GLuint FBO;
	GLuint quadVAO;
	std::shared_ptr<Shader> shader;

	FrameBuffer() : width(Screen::getScreenWidth()), height(Screen::getScreenHeight()) {};
	FrameBuffer(std::shared_ptr<Shader> shader, int width = Screen::getScreenWidth(), int height = Screen::getScreenHeight()) : shader(move(shader)), width(width), height(height) {	}

	TextureBuffer* getBuffer() {
		return buffer.get();
	}
	
	void mountFrameBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Screen::setViewport(width, height);
	}

	void unmountFrameBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		Screen::setViewport(Screen::getScreenWidth(), Screen::getScreenHeight());
	}

	virtual void executeFrameBuffer(Renderer& renderer) = 0;

protected:
	int width, height;
	std::unique_ptr<TextureBuffer> buffer;
	

	virtual void init() = 0;

	void initDrawingQuad() {
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
};

#endif