#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	static GLuint matricesBlockBinding;
	static GLuint lightBlockBinding;

	bool initialized = false;
	bool active = true;
	GLuint program;
	
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

	void registerAttribute(const char* attrib);
	void registerUniform(const char* uniform);
	
	GLuint getAttribLocation(const char* attrib);
	GLuint getUniformLocation(const char* unif);

	void use();

	//	UBO
	//	Model View Matricies
	void registerMatriciesUBO();
	static void initializeMatricesUBO();
	static void updateProjectionMatrix(glm::mat4 projection);
	static void updateViewMatrix(glm::mat4 view);
	//	Lights
	void registerLightsUBO();

private:
	std::map<std::string, GLuint> attribList;
	std::map<std::string, GLuint> unifLocationList;

	static GLuint matricesUBO;
};

#endif
