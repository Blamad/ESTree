#ifndef CONSOLE_COMPONENT_H
#define CONSOLE_COMPONENT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "UIComponent.h"
#include "Screen.h"
#include "Logger.h"
#include <memory>
#include <map>
#include <GLM\glm.hpp>
#include <GL\glew.h>

using namespace std;

struct Character {
	GLuint     textureID;  // ID handle of the glyph texture
	glm::ivec2 size;       // Size of glyph
	glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
	GLuint     advance;    // Offset to advance to next glyph
};

class ConsoleComponent : public UIComponent {
public:
	ConsoleComponent(vec3 fontColor = vec3(0.3, 0.7f, 0.9f)): fontColor(fontColor) { init(); }

	void init();
	void draw();
	void update(double &dt, InputState &inputState);

private:
	static Logger logger;
	GLuint VAO, VBO;
	map<GLchar, Character> characters;
	unique_ptr<Shader> fontShader;
	vec3 fontColor;


	void initFreeType();
	void renderLine(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
};

#endif