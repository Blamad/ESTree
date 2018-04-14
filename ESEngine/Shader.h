#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <boost\foreach.hpp>

using namespace std;

enum ShaderSubroutine {
	RENDER_PASS, SHADOW_DEPTH_PASS, SINGLE_MESH_MODE, INSTANCED_MESH_MODE
};

class Shader
{
public:
	static GLuint matricesBlockBinding;
	static GLuint lightBlockBinding;

	bool active = true;
	GLuint program;

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath);

	void registerAttribute(const char* attrib);
	void registerUniform(const char* uniform);
	void registerSubroutine(const char * subroutine, GLenum shaderType);

	GLuint getSubroutineLocation(const char* subroutine);
	GLuint getAttribLocation(const char* attrib);
	GLuint getUniformLocation(const char* unif);

	void setShaderSubroutine(ShaderSubroutine subroutine);

	void updateShaderSubroutine();

	void use();
	bool isInitializedBy(int initializer);
	void setInitializedBy(int initializer);

	//	UBO
	//	Model View Matricies
	void registerMatriciesUBO();
	static void initializeMatricesUBO();
	static void updateProjectionMatrix(glm::mat4 &projection);
	static void updateViewMatrix(glm::mat4 &view);
	//	Lights
	void registerLightsUBO();

private:
	map<string, GLuint> attribList;
	map<string, GLuint> unifLocationList;
	map<string, GLuint> subroutineList;
	map<int, bool> initializedBy;

	vector<ShaderSubroutine> shaderSubroutines;

	static GLuint matricesUBO;
};

#endif