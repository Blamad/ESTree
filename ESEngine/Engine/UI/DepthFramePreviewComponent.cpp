#include "DepthFramePreviewComponent.h"

void DepthFramePreviewComponent::draw() {
	if (!enabled)
		return;

	if (!initialized)
		init();
	shader->use();

	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, DepthFrameBuffer::getCurrentFrameTextureBuffer()->id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void DepthFramePreviewComponent::init() {
	shader = ShaderManager::getShader("Resources/Shaders/FontShader.vert", "Resources/Shaders/DepthQuadDebug.frag");
	glm::mat4 projection = glm::ortho(.0f, .0f + Screen::getScreenWidth(), .0f, .0f + Screen::getScreenHeight());

	shader->use();
	if (!shader->isInitializedBy(3)) {
		glUniformMatrix4fv(glGetUniformLocation(shader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		shader->setInitializedBy(3);
	}
	
	GLfloat vertices[6][4] = {
	{ xpos,			ypos + height,  0.0, 1.0 },
	{ xpos,			ypos,			0.0, 0.0 },
	{ xpos + width, ypos,			1.0, 0.0 },

	{ xpos,			ypos + height,  0.0, 1.0 },
	{ xpos + width, ypos,			1.0, 0.0 },
	{ xpos + width, ypos + height,  1.0, 1.0 }
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	initialized = true;
}