#ifndef CONSOLE_COMPONENT_H
#define CONSOLE_COMPONENT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ShaderManager.h"
#include "UIComponent.h"
#include "Screen.h"
#include "Logger.h"
#include "ConsoleInterpreter.h"
#include "Context.h"
#include "ConsoleMemory.h"
#include <memory>
#include <map>
#include <GLM\glm.hpp>
#include <chrono>
#include <boost\foreach.hpp>

#include <glad\glad.h>
#include <GLFW\glfw3.h>

struct Character {
	GLuint  textureID;
	glm::ivec2	size;
	glm::ivec2	bearing;
	GLuint	advance;    
};

class ConsoleComponent : public UIComponent {
public:
	ConsoleComponent(glm::vec3 fontColor = glm::vec3(1.0, .0f, .0f));

	void init();
	void draw();
	void update(double &dt, InputState &inputState);
	void writeLine(std::string line);
	ConsoleMemory* getConsoleMemory() { return memory.get(); }

private:
	static Logger logger;

	GLuint VAO, VBO;
	std::map<GLchar, Character> characters;
	std::shared_ptr<Shader> fontShader;
	glm::vec3 fontColor;
	bool isFocused;
	int consoleXPos;
	int consoleYPos;

	std::string inputLine;

	float lineOffset = 18.0f;
	float fontScale = 0.7f;
	int maxLines = 10;
	int consoleScrollOffset = 0;

	std::unique_ptr<ConsoleMemory> memory;
	std::unique_ptr<ConsoleInterpreter> interpreter;

	void initFreeType();
	void renderLine(std::string text, GLfloat x, GLfloat y);
	
	void processInputLine();

	void registerInputChar(char c);
	void removeLastCharFromInput(int key);
	void scrollUp();
	void scrollDown();
};

#endif