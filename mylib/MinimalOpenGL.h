#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

GLuint compileShaderStage(GLenum stage, const std::string& source);

GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& pixelShaderSource);

std::string loadTextFile(const std::string& filename);

GLenum checkFramebufferCompleteness();