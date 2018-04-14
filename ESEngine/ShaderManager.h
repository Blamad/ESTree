#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <memory>
#include <map>

#include "Shader.h"

using namespace std;

class ShaderManager {
public:
	static shared_ptr<Shader> getShader(const GLchar* vertexPath, const GLchar* fragmentPath);
	static shared_ptr<Shader> getShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath);

	void operator=(ShaderManager const&) = delete;
	ShaderManager(ShaderManager const&) = delete;
	~ShaderManager();

private:
	ShaderManager() {};
	map<string, shared_ptr<Shader>> shaders;

	static ShaderManager& getInstance();
	shared_ptr<Shader> findOrCreateShader(const GLchar* vertexPath, const GLchar* fragmentPath);
	shared_ptr<Shader> findOrCreateShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath);
};

#endif