#include "ShaderManager.h"

ShaderManager& ShaderManager::getInstance() {
	static ShaderManager instance;
	return instance;
}

std::shared_ptr<Shader> ShaderManager::getShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
	return getInstance().findOrCreateShader(vertexPath, fragmentPath);
}

std::shared_ptr<Shader> ShaderManager::getShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath) {
	return getInstance().findOrCreateShader(vertexPath, fragmentPath, geometryPath);
}

std::shared_ptr<Shader> ShaderManager::findOrCreateShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
	std::string shaderId = std::string(vertexPath) + "_" + std::string(fragmentPath);
	auto it = shaders.find(shaderId);

	if (it != shaders.end())
		return it->second;

	std::shared_ptr<Shader> shader = std::shared_ptr<Shader>(new Shader(vertexPath, fragmentPath));
	shaders[shaderId] = shader;

	return shader;
}

std::shared_ptr<Shader> ShaderManager::findOrCreateShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath) {
	std::string shaderId = std::string(vertexPath) + "_" + std::string(fragmentPath) + "_" + std::string(geometryPath);
	auto it = shaders.find(shaderId);

	if (it != shaders.end())
		return it->second;

	std::shared_ptr<Shader> shader = std::shared_ptr<Shader>(new Shader(vertexPath, fragmentPath, geometryPath));
	shaders[shaderId] = shader;

	return shader;
}

ShaderManager::~ShaderManager() {

}
