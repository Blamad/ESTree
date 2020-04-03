#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <memory>
#include <map>

#include "GraphicEngine/Core/Shader.h"

class ShaderManager {
public:
	static std::shared_ptr<Shader> getShader(const GLchar* vertexPath, const GLchar* fragmentPath);
	static std::shared_ptr<Shader> getShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath);

	void operator=(ShaderManager const&) = delete;
	ShaderManager(ShaderManager const&) = delete;
	~ShaderManager();

private:
	ShaderManager() {};
	std::map<std::string, std::shared_ptr<Shader>> shaders;

	static ShaderManager& getInstance();
	std::shared_ptr<Shader> findOrCreateShader(const GLchar* vertexPath, const GLchar* fragmentPath);
	std::shared_ptr<Shader> findOrCreateShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath);
};

#endif