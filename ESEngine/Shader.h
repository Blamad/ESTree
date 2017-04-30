#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <GL/glew.h>; // Include glew to get all the required OpenGL headers

class Shader
{
public:
	bool initialized = false;
	// The program ID
	GLuint Program;
	// Constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader();

	void RegisterAttribute(const char* attrib);
	void RegisterUniform(const char* uniform);

	GLuint GetAttribLocation(const char* attrib);
	GLuint GetUniformLocation(const char* unif);

	// Use the program
	void Use();

private:
	/*  Shader data  */
	std::map<std::string, GLuint> _attribList;
	std::map<std::string, GLuint> _unifLocationList;
};

#endif
