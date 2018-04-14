#include "ShaderManager.h"

ShaderManager& ShaderManager::getInstance() {
	static ShaderManager instance;
	return instance;
}

shared_ptr<Shader> ShaderManager::getShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
	return getInstance().findOrCreateShader(vertexPath, fragmentPath);
}

shared_ptr<Shader> ShaderManager::getShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath) {
	return getInstance().findOrCreateShader(vertexPath, fragmentPath, geometryPath);
}

shared_ptr<Shader> ShaderManager::findOrCreateShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
	string shaderId = string(vertexPath) + "_" + string(fragmentPath);
	auto it = shaders.find(shaderId);

	if (it != shaders.end())
		return it->second;

	shared_ptr<Shader> shader = shared_ptr<Shader>(new Shader(vertexPath, fragmentPath));
	shaders[shaderId] = shader;

	return shader;
}

shared_ptr<Shader> ShaderManager::findOrCreateShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath) {
	string shaderId = string(vertexPath) + "_" + string(fragmentPath) + "_" + string(geometryPath);
	auto it = shaders.find(shaderId);

	if (it != shaders.end())
		return it->second;

	shared_ptr<Shader> shader = shared_ptr<Shader>(new Shader(vertexPath, fragmentPath, geometryPath));
	shaders[shaderId] = shader;

	return shader;
}

ShaderManager::~ShaderManager() {

}
