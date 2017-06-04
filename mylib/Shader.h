#pragma once

#include "MinimalOpenGL.h"

class ShaderProgram {
	GLuint m_program;

public:
	ShaderProgram() {};
	ShaderProgram(const std::string& vertexShaderSource, const std::string& pixelShaderSource);
	void load(const std::string& vertexShaderSource, const std::string& pixelShaderSource);
	void use();
	void unuse();
	void setParameter1i(const std::string& name, int);
	void setParameter1f(const std::string& name, float);
	void setParameter2f(const std::string& name, float, float);
};