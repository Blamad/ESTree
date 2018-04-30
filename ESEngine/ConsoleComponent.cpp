#include "ConsoleComponent.h"

Logger ConsoleComponent::logger("ConsoleComponent");

ConsoleComponent::ConsoleComponent(vec3 fontColor) : fontColor(fontColor) {
	consoleXPos = Screen::getScreenWidth() * .01f;
	consoleYPos = Screen::getScreenHeight() * .01f;
	isFocused = false;
	//Context::setConsoleComponent(this);

	init();
}

void ConsoleComponent::init() {
	initFreeType();

	mat4 projection = glm::ortho(.0f, .0f + Screen::getScreenWidth(), .0f, .0f + Screen::getScreenHeight());

	fontShader = ShaderManager::getShader("Shaders/FontShader.vert", "Shaders/FontShader.frag");
	fontShader->use();
	if (!fontShader->isInitializedBy(3)) {
		glUniformMatrix4fv(glGetUniformLocation(fontShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		fontShader->registerUniform("textColor");
		fontShader->setInitializedBy(3);
	}

	lastInputIndex = 0;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ConsoleComponent::draw() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	fontShader->use();
	glUniform3f(fontShader->getUniformLocation("textColor"), fontColor.x, fontColor.y, fontColor.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	int index = 0;
	for (int i = 0; i < 10; i++) {
		index = textBuffer.size() - 1 - i;
		if (index < 0)
			continue;
		renderLine(textBuffer[index], consoleXPos, consoleYPos + lineOffset * (i + 1));
	}

	string inputLineDisplayed = ">" + inputLine + (isFocused ? "_" : "");
	renderLine(inputLineDisplayed, consoleXPos, consoleYPos);

	glBindVertexArray(0);
	glDisable(GL_BLEND);
}

void ConsoleComponent::update(double &dt, InputState &inputState) {
	BOOST_FOREACH(auto keyEvent, inputState.getKeyboardEvents()) {
		if (keyEvent.key == GLFW_KEY_GRAVE_ACCENT && keyEvent.state == PRESSED) {
			isFocused = !isFocused;
			inputLine.clear();
			lastInputIndex = 0;
			return;
		}
	}
	
	if (isFocused) {
		if (!inputState.getMouseClickEvents().empty()) {
			isFocused = false;
			inputLine.clear();
			lastInputIndex = 0;
			return;
		}

		BOOST_FOREACH(auto charEvent, inputState.getCharacterEvents()) {
			if (charEvent != GLFW_KEY_ENTER) {
				registerInputChar(charEvent);
			}
		}
		BOOST_FOREACH(auto keyEvent, inputState.getKeyboardEvents()) {
			if (keyEvent.key == GLFW_KEY_BACKSPACE && keyEvent.state != RELEASED)
				removeLastCharFromInput(keyEvent.key);
			if (keyEvent.key == GLFW_KEY_ENTER && keyEvent.state == PRESSED) {
				processInputLine();
				lastInputIndex = 0;
			}
			if (keyEvent.key == GLFW_KEY_UP && keyEvent.state == PRESSED && !inputHistory.empty()) {
				lastInputIndex--;
				if (lastInputIndex < 0)
					lastInputIndex = inputHistory.size() - 1;
				inputLine = inputHistory[lastInputIndex];
			}
			if (keyEvent.key == GLFW_KEY_DOWN && keyEvent.state == PRESSED && !inputHistory.empty()) {
				lastInputIndex++;
				if (lastInputIndex > inputHistory.size() - 1)
					lastInputIndex = 0;
				inputLine = inputHistory[lastInputIndex];
			}
		}
		inputState.blockEvents();
	}
}

void ConsoleComponent::writeLine(string line) {
	textBuffer.push_back(line);
}

void ConsoleComponent::processInputLine() {
	textBuffer.push_back(inputLine);
	inputHistory.push_back(inputLine);
	ConsoleInterpreter::processInput(inputLine);
	inputLine.clear();
}

void ConsoleComponent::registerInputChar(char c) {
	inputLine.push_back(c);
}

void ConsoleComponent::removeLastCharFromInput(int key) {
	inputLine = inputLine.substr(0, inputLine.size() - 1);
}

void ConsoleComponent::renderLine(std::string text, GLfloat x, GLfloat y) {
	string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = characters[*c];

		GLfloat xpos = x + ch.bearing.x * fontScale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * fontScale;

		GLfloat w = ch.size.x * fontScale;
		GLfloat h = ch.size.y * fontScale;

		GLfloat vertices[6][4] = {
		{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos,     ypos,       0.0, 1.0 },
		{ xpos + w, ypos,       1.0, 1.0 },

		{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos + w, ypos,       1.0, 1.0 },
		{ xpos + w, ypos + h,   1.0, 0.0 }
		};

		glBindTexture(GL_TEXTURE_2D, ch.textureID);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.advance >> 6) * fontScale;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ConsoleComponent::initFreeType() {
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		logger.log(ERROR, "Could not init FreeType Library");
	FT_Face face;
	if (FT_New_Face(ft, "Fonts/arial.ttf", 0, &face))
		logger.log(ERROR, "Failed to load font");
	FT_Set_Pixel_Sizes(face, 0, 24);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			logger.log(ERROR, "Failed to load Glyph");
			continue;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture,
			ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		characters.insert(std::pair<GLchar, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}