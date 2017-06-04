
#include "MinimalOpenGL.h"

#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>

GLuint compileShaderStage(GLenum stage, const std::string& source) {
	GLuint shader = glCreateShader(stage);
	const char* srcArray[] = { source.c_str() };

	glShaderSource(shader, 1, srcArray, NULL);
	glCompileShader(shader);

	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		GLint logSize = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

		std::vector<GLchar> errorLog(logSize);
		glGetShaderInfoLog(shader, logSize, &logSize, &errorLog[0]);

		fprintf(stderr, "Error while compiling\n %s\n\nError: %s\n", source.c_str(), &errorLog[0]);
		assert(false);

		glDeleteShader(shader);
		shader = GL_NONE;
	}

	return shader;
}


GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& pixelShaderSource) {
	GLuint shader = glCreateProgram();

	glAttachShader(shader, compileShaderStage(GL_VERTEX_SHADER, vertexShaderSource));
	glAttachShader(shader, compileShaderStage(GL_FRAGMENT_SHADER, pixelShaderSource));
	glLinkProgram(shader);

	return shader;
}

std::string loadTextFile(const std::string& filename) {
	std::stringstream buffer;
	buffer << std::ifstream(filename.c_str()).rdbuf();
	return buffer.str();
}


GLenum checkFramebufferCompleteness() {
	GLenum fboStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (fboStatus)
		{
		case GL_FRAMEBUFFER_UNDEFINED:
			// Oops, no window exists?
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			// Check the status of each attachment
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			// Attach at least one buffer to the FBO
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			// Check that all attachments enabled via
			// glDrawBuffers exist in FBO
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			// Check that the buffer specified via
			// glReadBuffer exists in FBO
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			// Reconsider formats used for attached buffers
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			// Make sure the number of samples for each
			// attachment is the same
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			// Make sure the number of layers for each
			// attachment is the same
			break;
		}
	}
	return fboStatus;
}