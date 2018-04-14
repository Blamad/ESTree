#include "Shader.h"

GLuint Shader::matricesBlockBinding = 0;
GLuint Shader::lightBlockBinding = 1;

GLuint Shader::matricesUBO = 0;

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensures ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into GLchar array
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	// 2. Compile shaders
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];

	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// Print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	// Similiar for Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// Print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	// Shader Program
	this->program = glCreateProgram();
	glAttachShader(this->program, vertex);
	glAttachShader(this->program, fragment);
	glLinkProgram(this->program);
	// Print linking errors if any
	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	active = true;
}

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath)
{
	std::string vertexCode, fragmentCode, geometryCode;
	std::ifstream vShaderFile, fShaderFile, gShaderFile;
	
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		gShaderFile.open(geometryPath);
		std::stringstream vShaderStream, fShaderStream, gShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		gShaderStream << gShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		gShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		geometryCode = gShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();
	const GLchar* gShaderCode = geometryCode.c_str();

	GLuint vertex, fragment, geometry;
	GLint success;
	GLchar infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &gShaderCode, NULL);
	glCompileShader(geometry);

	glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(geometry, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	this->program = glCreateProgram();
	glAttachShader(this->program, vertex);
	glAttachShader(this->program, fragment);
	glAttachShader(this->program, geometry);
	glLinkProgram(this->program);
	
	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);

	active = true;
}

void Shader::use() {
	glUseProgram(program);
}

void Shader::registerAttribute(const char* attrib) {
	attribList[attrib] = glGetAttribLocation(program, attrib);
}

void Shader::registerUniform(const char* unif) {
	unifLocationList[unif] = glGetUniformLocation(program, unif);
}

void Shader::registerSubroutine(const char* subroutine, GLenum shaderType) {
	subroutineList[subroutine] = glGetSubroutineIndex(program, shaderType, subroutine);
}

GLuint Shader::getAttribLocation(const char* attrib) {
	return attribList[attrib];
}

GLuint Shader::getUniformLocation(const char* unif) {
	return unifLocationList[unif];
}

GLuint Shader::getSubroutineLocation(const char* subroutine) {
	return subroutineList[subroutine];
}

void Shader::registerMatriciesUBO() {
	GLuint uniformBlockIndex = glGetUniformBlockIndex(program, "Matrices");
	glUniformBlockBinding(program, uniformBlockIndex, matricesBlockBinding);
}

void Shader::initializeMatricesUBO() {
	glGenBuffers(1, &matricesUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, matricesBlockBinding, matricesUBO, 0, 2 * sizeof(glm::mat4));
}

void Shader::updateProjectionMatrix(glm::mat4 projection) {
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Shader::updateViewMatrix(glm::mat4 view) {
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Shader::registerLightsUBO() {
	GLuint uniformBlockIndex = glGetUniformBlockIndex(program, "Lights");
	glUniformBlockBinding(program, uniformBlockIndex, lightBlockBinding);
}

void Shader::updateShaderSubroutine() {

	GLuint indexes[2] = { getSubroutineLocation("renderPass"), getSubroutineLocation("singleMesh") };
	BOOST_FOREACH(ShaderSubroutine shaderSubroutine, shaderSubroutines) {
		switch (shaderSubroutine) {
		case SHADOW_DEPTH_PASS:
			indexes[0] = getSubroutineLocation("shadowDepthPass");
			break;
		case INSTANCED_MESH_MODE:
			indexes[1] = getSubroutineLocation("instancedMesh");
			break;
		default:
			break;
		}
	}

	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &indexes[0]);
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &indexes[1]);
	shaderSubroutines.clear();
}

void Shader::setShaderSubroutine(ShaderSubroutine subroutine) {
	this->shaderSubroutines.push_back(subroutine);
}

