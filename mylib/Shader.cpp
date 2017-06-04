
#include "Shader.h"



ShaderProgram::ShaderProgram(const std::string& vertexFile, const std::string& fragmentFile) {
	m_program = createShaderProgram(loadTextFile(vertexFile), loadTextFile(fragmentFile));
}

void ShaderProgram::load(const std::string& vertexFile, const std::string& fragmentFile) {
	m_program = createShaderProgram(loadTextFile(vertexFile), loadTextFile(fragmentFile));
}

void ShaderProgram::use() {
	glUseProgram(m_program);
}

void ShaderProgram::unuse() {
	glUseProgram(0);
}

void ShaderProgram::setParameter1i(const std::string& name, int val) {
	const GLint loc = glGetUniformLocation(m_program, name.c_str());
	glUniform1i(loc, val);
}

void ShaderProgram::setParameter1f(const std::string& name, float val) {
	const GLint loc = glGetUniformLocation(m_program, name.c_str());
	glUniform1f(loc, val);
}

void ShaderProgram::setParameter2f(const std::string& name, float val0, float val1) {
	const GLint loc = glGetUniformLocation(m_program, name.c_str());
	glUniform2f(loc, val0, val1);
}