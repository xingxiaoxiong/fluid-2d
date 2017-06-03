
#include "Fluid2D.h"
#include <vector>

Fluid2D::Fluid2D(int width, int height) : 
	m_bApplyForce(false), 
	m_v_src(0), 
	m_v_dst(1), 
	m_p_src(0), 
	m_p_dst(1), 
	m_dye_src(0), 
	m_dye_dst(1), 
	m_viscosity(0.2) {
	m_width = width;
	m_height = height;

	m_line = std::make_unique<Line>();
	m_quad = std::make_unique<Quad>();

	setup();
}

void Fluid2D::setup() {
	// initialize line, quad
	// ---- m_line might not be useful
	/*float xMin = -1.f + 1.0 / m_width;
	float xMax = 1.f - 1.0 / m_width;*/
	float xMin = -1.f;
	float xMax = 1.f;
	const float position[] = { xMin, xMax, -0.5f, xMin, xMin, -0.5f, xMax, xMin, -0.5f, xMax, xMax, -0.5f };
	const float texCoord[] = { 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 1.f };
	const int   boundaryIndex[] = { 0, 1, 2, 3, 0 };
	const int   quadIndex[] = { 0, 1, 3, 3, 1, 2 };

	std::vector<float> posVector(position, position + sizeof(position) / sizeof(position[0]));
	std::vector<float> texVector(texCoord, texCoord + sizeof(texCoord) / sizeof(texCoord[0]));
	std::vector<unsigned int> lineIndexVector(boundaryIndex, boundaryIndex + sizeof(boundaryIndex) / sizeof(boundaryIndex[0]));
	std::vector<unsigned int> quadIndexVector(quadIndex, quadIndex + sizeof(quadIndex) / sizeof(quadIndex[0]));

	m_line->loadData(posVector, texVector, lineIndexVector);
	m_quad->loadData(posVector, texVector, quadIndexVector);
	
	// initialize framebuffer object
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	std::vector<GLfloat> emptyData(m_width * m_height * 3, 0.0);
	std::vector<GLfloat> flowRight(m_width * m_height * 3, 0.0);
	std::vector<GLfloat> dyeTexture(m_width * m_height * 3, 0);

	for (auto i = 0; i < m_height; i++) {
		for (auto j = 0; j < m_width; j++) {
			flowRight[(i * m_width + j) * 3 + 0] = 0.1;
		}
	}

	for (auto i = 0; i < m_height; i++) {
		for (auto j = 0; j < m_width; j++) {
			float color = glm::distance(glm::vec2(i, j), glm::vec2(m_width * 0.5, m_height * 0.5));
			for (int c = 0; c < 3; c++) {
				dyeTexture[(i * m_width + j) * 3 + c] = glm::min(color / 255, 1.0f);
			}
		}
	}

	glGenTextures(2, m_velocity);
	for (auto i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, m_velocity[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_width, m_height, 0, GL_RGB, GL_FLOAT, &emptyData[0]);
	}

	glGenTextures(2, m_pressure);
	for (auto i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, m_pressure[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_width, m_height, 0, GL_R, GL_FLOAT, &emptyData[0]);
	}

	glGenTextures(1, &m_divergence);
	glBindTexture(GL_TEXTURE_2D, m_divergence);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_width, m_height, 0, GL_R, GL_FLOAT, 0);

	glGenTextures(2, m_dye);
	for (auto i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, m_dye[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_width, m_height, 0, GL_RGB, GL_FLOAT, &dyeTexture[0]);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_velocity[0], 0);

	auto fboStatus = checkFramebufferCompleteness();
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// initialize shaders
	testShader = createShaderProgram(loadTextFile("shaders/poly.vs"), loadTextFile("shaders/poly.fs"));
	m_boundaryAdvectShader = createShaderProgram(loadTextFile("shaders/boundary_advect.vs"), loadTextFile("shaders/boundary_advect.fs"));
	m_interiorAdvectShader = createShaderProgram(loadTextFile("shaders/interior_advect.vs"), loadTextFile("shaders/interior_advect.fs"));
	m_applyForceShader = createShaderProgram(loadTextFile("shaders/apply_force.vs"), loadTextFile("shaders/apply_force.fs"));
	m_copyShader = createShaderProgram(loadTextFile("shaders/copy.vs"), loadTextFile("shaders/copy.fs"));
	m_possionShader = createShaderProgram(loadTextFile("shaders/solve_jacobi.vs"), loadTextFile("shaders/solve_jacobi.fs"));
	m_divergenceShader = createShaderProgram(loadTextFile("shaders/divergence.vs"), loadTextFile("shaders/divergence.fs"));
	m_gradientSubtractionShader = createShaderProgram(loadTextFile("shaders/gradient_subtraction.vs"), loadTextFile("shaders/gradient_subtraction.fs"));
}

void Fluid2D::applyForce(float startX, float startY, float deltaX, float deltaY) {
	m_forceX = 2 * deltaX / m_width;
	m_forceY = -2 * deltaY / m_height;
	m_forceStartX = startX / m_width * 2 - 1;
	m_forceStartY = -(startY / m_height * 2 - 1);

	m_bApplyForce = true;
}

void Fluid2D::update(float timeStep) {
	advect(0.0001);

	if (m_bApplyForce) {
		//testApplyForce(m_forceX, m_forceY, m_forceStartX, m_forceStartY);
		applyForce();
		m_bApplyForce = false;
	}

	diffuseVelocity(timeStep);

	divergence();

	computePressure();

	gradientSubtraction();
	
	// display intermediate result
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//GLenum drawBuffers[] = { GL_BACK_LEFT };
	//glDrawBuffers(1, drawBuffers);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_dye[m_dye_src], 0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, m_width, m_height,
		0, 0, m_width * 0.5, m_height * 0.5, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_dye[m_dye_dst], 0);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glBlitFramebuffer(0, 0, m_width, m_height,
		m_width * 0.5, 0, m_width, m_height * 0.5, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_velocity[m_v_src], 0);
	glReadBuffer(GL_COLOR_ATTACHMENT2);
	glBlitFramebuffer(0, 0, m_width, m_height,
		0, m_height * 0.5, m_width * 0.5, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_velocity[m_v_dst], 0);
	glReadBuffer(GL_COLOR_ATTACHMENT3);
	glBlitFramebuffer(0, 0, m_width, m_height,
		m_width * 0.5, m_height * 0.5, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	
}

void Fluid2D::advect(float timeStep) {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_velocity[m_v_dst], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_dye[m_dye_dst], 0);

	GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, DrawBuffers);

	checkFramebufferCompleteness();

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_velocity[m_v_src]);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, m_dye[m_dye_src]);
	

	// boundary
	{
		glUseProgram(m_boundaryAdvectShader);

		const GLint textureLoc = glGetUniformLocation(m_boundaryAdvectShader, "field");
		const GLint dyeLoc = glGetUniformLocation(m_boundaryAdvectShader, "dye");
		const GLint scaleLoc = glGetUniformLocation(m_boundaryAdvectShader, "scale");
		const GLint halfTexelWidthLoc = glGetUniformLocation(m_boundaryAdvectShader, "halfTexelWidth");

		glUniform1f(scaleLoc, -1);
		glUniform1i(textureLoc, 0);
		glUniform1i(dyeLoc, 1);
		glUniform1f(halfTexelWidthLoc, 1.0 / m_width * 0.5);

		m_quad->draw();
		glUseProgram(0);
	}

	// interior
	{
		glUseProgram(m_interiorAdvectShader);

		const GLint timeStepLoc = glGetUniformLocation(m_interiorAdvectShader, "timeStep");
		const GLint rdxLoc = glGetUniformLocation(m_interiorAdvectShader, "rdx");
		const GLint uLoc = glGetUniformLocation(m_interiorAdvectShader, "u");
		//const GLint xLoc = glGetUniformLocation(m_interiorAdvectShader, "x");
		const GLint dyeLoc = glGetUniformLocation(m_boundaryAdvectShader, "dye");
		const GLint halfTexelWidthLoc = glGetUniformLocation(m_interiorAdvectShader, "halfTexelWidth");

		glUniform1f(timeStepLoc, timeStep);
		glUniform1f(rdxLoc, m_width / 2.0);
		glUniform1f(halfTexelWidthLoc, 1.0 / m_width * 0.5);
		glUniform1i(uLoc, 0);
		//glUniform1i(xLoc, 0);
		glUniform1i(dyeLoc, 1);

		m_quad->draw();
		glUseProgram(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::swap(m_v_src, m_v_dst);
	std::swap(m_dye_src, m_dye_dst);
}

void Fluid2D::applyForce() {
	float radius = glm::sqrt(m_forceX * m_forceX + m_forceY * m_forceY);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_velocity[m_v_dst], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_velocity[m_v_src]);

	glUseProgram(m_applyForceShader);

	const GLint radiusLoc = glGetUniformLocation(m_applyForceShader, "radius");
	const GLint pointLoc = glGetUniformLocation(m_applyForceShader, "point");
	const GLint uLoc = glGetUniformLocation(m_applyForceShader, "u");
	const GLint FLoc = glGetUniformLocation(m_applyForceShader, "F");
	const GLint halfTexelWidthLoc = glGetUniformLocation(m_applyForceShader, "halfTexelWidth");

	glUniform1f(radiusLoc, radius);
	glUniform2f(pointLoc, m_forceStartX, m_forceStartY);
	glUniform1f(halfTexelWidthLoc, 1.0 / 1024);
	glUniform1i(uLoc, 0);
	glUniform2f(FLoc, m_forceX, m_forceY);

	m_quad->draw();
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::swap(m_v_src, m_v_dst);
}

void Fluid2D::diffuseVelocity(float timeStep) {
	for (int i = 0; i < 50; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_velocity[m_v_dst], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_velocity[m_v_src]);

		glUseProgram(m_possionShader);

		const GLint xLoc = glGetUniformLocation(m_possionShader, "x");
		const GLint bLoc = glGetUniformLocation(m_possionShader, "b");
		const GLint alphaLoc = glGetUniformLocation(m_possionShader, "alpha");
		const GLint rBetaLoc = glGetUniformLocation(m_possionShader, "rBeta");
		const GLint halfTexelWidthLoc = glGetUniformLocation(m_possionShader, "halfTexelWidth");

		float dx = 2.0 / m_width;
		float alpha = dx * dx / (m_viscosity * timeStep);
		float rBeta = 1.0f / (4.0f + alpha);

		glUniform1i(xLoc, 0);
		glUniform1i(bLoc, 0);
		glUniform1f(halfTexelWidthLoc, 1.0 / m_width * 0.5);
		glUniform1f(alphaLoc, alpha);
		glUniform1f(rBetaLoc, rBeta);

		m_quad->draw();
		glUseProgram(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		std::swap(m_v_src, m_v_dst);
	}
}

void Fluid2D::divergence() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_divergence, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_velocity[m_v_src]);

	glUseProgram(m_divergenceShader);

	const GLint wLoc = glGetUniformLocation(m_divergenceShader, "w");
	const GLint halfrdxLoc = glGetUniformLocation(m_divergenceShader, "halfrdx");
	const GLint halfTexelWidthLoc = glGetUniformLocation(m_divergenceShader, "halfTexelWidth");

	float halfrdx = 2.0 / m_width * 0.5;

	glUniform1i(wLoc, 0);
	glUniform1f(halfTexelWidthLoc, 1.0 / m_width * 0.5);
	glUniform1f(halfrdxLoc, halfrdx);

	m_quad->draw();
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Fluid2D::computePressure() {
	for (int i = 0; i < 80; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pressure[m_p_dst], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_pressure[m_p_src]);

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, m_divergence);

		glUseProgram(m_possionShader);

		const GLint xLoc = glGetUniformLocation(m_possionShader, "x");
		const GLint bLoc = glGetUniformLocation(m_possionShader, "b");
		const GLint alphaLoc = glGetUniformLocation(m_possionShader, "alpha");
		const GLint rBetaLoc = glGetUniformLocation(m_possionShader, "rBeta");
		const GLint halfTexelWidthLoc = glGetUniformLocation(m_possionShader, "halfTexelWidth");

		float dx = 2.0 / m_width;
		float alpha = - dx*dx;
		float rBeta = 0.25;

		glUniform1i(xLoc, 0);
		glUniform1i(bLoc, 1);
		glUniform1f(halfTexelWidthLoc, 1.0 / m_width * 0.5);
		glUniform1f(alphaLoc, alpha);
		glUniform1f(rBetaLoc, rBeta);

		m_quad->draw();
		glUseProgram(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		std::swap(m_p_src, m_p_dst);
	}
}

void Fluid2D::gradientSubtraction() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_velocity[m_v_dst], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_velocity[m_v_src]);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, m_pressure[m_p_src]);

	glUseProgram(m_gradientSubtractionShader);

	const GLint wLoc = glGetUniformLocation(m_gradientSubtractionShader, "w");
	const GLint pLoc = glGetUniformLocation(m_gradientSubtractionShader, "p");
	const GLint halfrdxLoc = glGetUniformLocation(m_gradientSubtractionShader, "halfrdx");
	const GLint halfTexelWidthLoc = glGetUniformLocation(m_gradientSubtractionShader, "halfTexelWidth");

	float halfrdx = 2.0 / m_width * 0.5;

	glUniform1i(wLoc, 0);
	glUniform1i(pLoc, 1);
	glUniform1f(halfTexelWidthLoc, 1.0 / m_width * 0.5);
	glUniform1f(halfrdxLoc, halfrdx);

	m_quad->draw();
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	std::swap(m_v_src, m_v_dst);
}

void Fluid2D::solvePossion(int iterationCount = 30) {

}

void Fluid2D::copy(const GLuint* two, unsigned int from, unsigned int to) {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, two[to], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, two[from]);
	glUseProgram(m_copyShader);

	const GLint sourceLoc = glGetUniformLocation(m_copyShader, "source");
	glUniform1i(sourceLoc, 0);

	m_quad->draw();
	glUseProgram(0);
}


// *************************
// *** Testing Functions ***
// *************************
void Fluid2D::testBoundaryDrawing() {
	glUseProgram(testShader);
	m_quad->draw();
	glUseProgram(0);
}

void Fluid2D::testBoundaryAdvect() {
	std::vector<GLfloat> data(m_width * m_height * 3, 0.0);
	for (auto i = 0; i < m_width; i++) {
		for (auto j = 0; j < m_height; j++) {
			if (i == 0 || i == m_width - 1 || j == 0 || j == m_height - 1) {

			}
			else {
				data[(i * m_height + j) * 3 + 1] = 1.0;
			}
		}
	}

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, m_velocity[0]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, &data[0]);

	glUseProgram(m_boundaryAdvectShader);

	const GLint textureLoc = glGetUniformLocation(m_boundaryAdvectShader, "field");
	const GLint scaleLoc = glGetUniformLocation(m_boundaryAdvectShader, "scale");
	const GLint halfTexelWidthLoc = glGetUniformLocation(m_boundaryAdvectShader, "halfTexelWidth");

	glUniform1f(scaleLoc, 1);
	glUniform1i(textureLoc, 0);
	glUniform1f(halfTexelWidthLoc, 1.0 / 1024);

	m_quad->draw();
	glUseProgram(0);
}

void Fluid2D::testInteriorAdvect(float timeStep) {
	std::vector<GLfloat> data(m_width * m_height * 3, 0.0);
	for (auto i = 0; i < m_width; i++) {
		for (auto j = 0; j < m_height; j++) {
			if (i < m_width * 0.5 && j < m_height * 0.5) {
				data[(i * m_height + j) * 3] = 0.01;
			}
		}
	}

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, m_velocity[0]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, &data[0]);

	glUseProgram(m_interiorAdvectShader);

	const GLint timeStepLoc = glGetUniformLocation(m_interiorAdvectShader, "timeStep");
	const GLint rdxLoc = glGetUniformLocation(m_interiorAdvectShader, "rdx");
	const GLint uLoc = glGetUniformLocation(m_interiorAdvectShader, "u");
	const GLint xLoc = glGetUniformLocation(m_interiorAdvectShader, "x");
	const GLint halfTexelWidthLoc = glGetUniformLocation(m_interiorAdvectShader, "halfTexelWidth");

	glUniform1f(timeStepLoc, timeStep);
	glUniform1f(rdxLoc, m_width / 2);
	glUniform1f(halfTexelWidthLoc, 1.0 / 1024);
	glUniform1i(uLoc, 0);
	glUniform1i(xLoc, 0);

	m_quad->draw();
	glUseProgram(0);
}

void Fluid2D::testApplyForce(float startX, float startY, float deltaX, float deltaY) {
	float radius = glm::sqrt(deltaX * deltaX + deltaY * deltaY);

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, m_velocity[0]);

	glUseProgram(m_applyForceShader);

	const GLint radiusLoc = glGetUniformLocation(m_applyForceShader, "radius");
	const GLint pointLoc = glGetUniformLocation(m_applyForceShader, "point");
	const GLint uLoc = glGetUniformLocation(m_applyForceShader, "u");
	const GLint FLoc = glGetUniformLocation(m_applyForceShader, "F");
	const GLint halfTexelWidthLoc = glGetUniformLocation(m_applyForceShader, "halfTexelWidth");

	glUniform1f(radiusLoc, radius);
	glUniform2f(pointLoc, startX, startY);
	glUniform1f(halfTexelWidthLoc, 1.0 / 1024);
	glUniform1i(uLoc, 0);
	glUniform2f(FLoc, deltaX, deltaY);

	m_quad->draw();
	glUseProgram(0);
}

void Fluid2D::testDye() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, m_velocity[0]);
	glUseProgram(m_copyShader);

	const GLint sourceLoc = glGetUniformLocation(m_copyShader, "source");
	glUniform1i(sourceLoc, 0);

	m_quad->draw();
	glUseProgram(0);
}