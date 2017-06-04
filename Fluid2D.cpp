
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
	m_viscosity(0.01),
	m_dx(1.0) {

	m_width = width;
	m_height = height;

	m_line = std::make_unique<Line>();
	m_quad = std::make_unique<Quad>();

	setup();
}

void Fluid2D::setup() {
	// initialize line, quad
	// ---- m_line might not be useful
	float xMin = -1.f;
	float xMax = 1.f;
	const float position[] = { xMin, xMax, -0.5f, xMin, xMin, -0.5f, xMax, xMin, -0.5f, xMax, xMax, -0.5f };
	const float texCoord[] = { -0.5f, m_width - 1.f + 0.5, -0.5f, -0.5f, m_width - 1.f + 0.5f, -0.5f,m_width - 1.f + 0.5f, m_width - 1.f + 0.5f };
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
			flowRight[(i * m_width + j) * 3 + 0] = 1.0;
		}
	}

	for (auto i = 0; i < m_height; i++) {
		for (auto j = 0; j < m_width; j++) {
			float color = glm::distance(glm::vec2(i, j), glm::vec2(m_width * 0.5, m_height * 0.5));
			for (int c = 0; c < 3; c++) {
				dyeTexture[(i * m_width + j) * 3 + c] = glm::min(color / 600, 1.0f);
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
	testShader.load("shaders/forward_uv.vs", "shaders/poly.fs");
	m_boundaryAdvectShader.load("shaders/forward_uv.vs", "shaders/boundary_advect.fs");
	m_interiorAdvectShader.load("shaders/forward_uv.vs", "shaders/interior_advect.fs");
	m_applyForceShader.load("shaders/forward_uv.vs", "shaders/apply_force.fs");
	m_copyShader.load("shaders/forward_uv.vs", "shaders/copy.fs");
	m_possionShader.load("shaders/forward_uv.vs", "shaders/solve_jacobi.fs");
	m_divergenceShader.load("shaders/forward_uv.vs", "shaders/divergence.fs");
	m_gradientSubtractionShader.load("shaders/forward_uv.vs", "shaders/gradient_subtraction.fs");
}

void Fluid2D::applyForce(float startX, float startY, float deltaX, float deltaY) {
	m_forceX = deltaX;
	m_forceY = -deltaY;
	m_forceStartX = startX;
	m_forceStartY = m_height - 1 - startY;

	m_bApplyForce = true;
}

void Fluid2D::update(float timeStep) {
	timeStep = 0.3;
	advectVelocity(timeStep);

	advect_dye(timeStep);

	if (m_bApplyForce) {
		applyForce();
		m_bApplyForce = false;
	}

	diffuseVelocity(timeStep);

	divergence();

	computePressure();

	gradientSubtraction();
	
	// display intermediate result
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);

	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_dye[m_dye_src], 0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, m_width, m_height,
		0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	return;
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

void Fluid2D::advectVelocity(float timeStep) {
	
	// boundary
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_velocity[m_v_dst], 0);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_velocity[m_v_src]);

		m_boundaryAdvectShader.use();

		m_boundaryAdvectShader.setParameter1f("scale", -1);
		m_boundaryAdvectShader.setParameter1i("field", 0);
		m_boundaryAdvectShader.setParameter1i("textureWidth", m_width);

		m_quad->draw();
		m_boundaryAdvectShader.unuse();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		std::swap(m_v_src, m_v_dst);
	}

	// interior
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_velocity[m_v_dst], 0);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_velocity[m_v_src]);

		m_interiorAdvectShader.use();

		m_interiorAdvectShader.setParameter1f("timeStep", timeStep);
		m_interiorAdvectShader.setParameter1f("rdx", 1.0 / m_dx);
		m_interiorAdvectShader.setParameter1i("textureWidth", m_width);
		m_interiorAdvectShader.setParameter1i("u", 0);
		m_interiorAdvectShader.setParameter1i("x", 0);

		m_quad->draw();
		m_interiorAdvectShader.unuse();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		std::swap(m_v_src, m_v_dst);
	}

}

void Fluid2D::advect_dye(float timeStep) {
	// boundary
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_dye[m_dye_dst], 0);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_dye[m_dye_src]);

		m_boundaryAdvectShader.use();

		m_boundaryAdvectShader.setParameter1f("scale", 0);
		m_boundaryAdvectShader.setParameter1i("field", 0);
		m_boundaryAdvectShader.setParameter1i("textureWidth", m_width);

		m_quad->draw();
		m_boundaryAdvectShader.unuse();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		std::swap(m_dye_src, m_dye_dst);
	}

	// interior
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_dye[m_dye_dst], 0);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_dye[m_dye_src]);

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, m_velocity[m_v_src]);

		m_interiorAdvectShader.use();

		m_interiorAdvectShader.setParameter1f("timeStep", timeStep);
		m_interiorAdvectShader.setParameter1f("rdx", 1.0 / m_dx);
		m_interiorAdvectShader.setParameter1i("textureWidth", m_width);
		m_interiorAdvectShader.setParameter1i("u", 1);
		m_interiorAdvectShader.setParameter1i("x", 0);

		m_quad->draw();
		m_interiorAdvectShader.unuse();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		std::swap(m_dye_src, m_dye_dst);
	}
}

void Fluid2D::applyForce() {
	float radius = glm::sqrt(m_forceX * m_forceX + m_forceY * m_forceY);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_velocity[m_v_dst], 0);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_velocity[m_v_src]);

	m_applyForceShader.use();

	m_applyForceShader.setParameter1f("radius", radius);
	m_applyForceShader.setParameter2f("point", m_forceStartX, m_forceStartY);
	m_applyForceShader.setParameter1i("textureWidth", m_width);
	m_applyForceShader.setParameter1i("u", 0);
	m_applyForceShader.setParameter2f("F", m_forceX, m_forceY);

	m_quad->draw();
	m_applyForceShader.unuse();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::swap(m_v_src, m_v_dst);
}

void Fluid2D::diffuseVelocity(float timeStep) {
	for (int i = 0; i < 60; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_velocity[m_v_dst], 0);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_velocity[m_v_src]);

		m_possionShader.use();

		float alpha = m_dx * m_dx / (m_viscosity * timeStep);
		float rBeta = 1.0f / (4.0f + alpha);

		m_possionShader.setParameter1i("x", 0);
		m_possionShader.setParameter1i("b", 0);
		m_possionShader.setParameter1i("textureWidth", m_width);
		m_possionShader.setParameter1f("alpha", alpha);
		m_possionShader.setParameter1f("rBeta", rBeta);

		m_quad->draw();
		m_possionShader.unuse();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		std::swap(m_v_src, m_v_dst);
	}
}

void Fluid2D::divergence() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_divergence, 0);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_velocity[m_v_src]);

	m_divergenceShader.use();

	float halfrdx = 0.5f / m_dx;
	m_divergenceShader.setParameter1i("w", 0);
	m_divergenceShader.setParameter1i("textureWidth", m_width);
	m_divergenceShader.setParameter1f("halfrdx", halfrdx);

	m_quad->draw();
	m_divergenceShader.unuse();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Fluid2D::computePressure() {
	std::vector<GLfloat> data(m_width * m_height * 3, 0.0);

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, m_pressure[m_p_src]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_R, GL_FLOAT, &data[0]);

	for (int i = 0; i < 70; i++) {
		{
			// boundary
			glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pressure[m_p_dst], 0);

			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, m_pressure[m_p_src]);

			m_boundaryAdvectShader.use();

			m_boundaryAdvectShader.setParameter1f("scale", 1);
			m_boundaryAdvectShader.setParameter1i("field", 0);
			m_boundaryAdvectShader.setParameter1i("textureWidth", m_width);

			m_quad->draw();
			m_boundaryAdvectShader.unuse();

			std::swap(m_p_src, m_p_dst);
		}

		{
			// interior
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pressure[m_p_dst], 0);

			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, m_pressure[m_p_src]);

			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, m_divergence);

			m_possionShader.use();

			float alpha = -m_dx * m_dx;
			float rBeta = 0.25f;

			m_possionShader.setParameter1i("x", 0);
			m_possionShader.setParameter1i("b", 1);
			m_possionShader.setParameter1i("textureWidth", m_width);
			m_possionShader.setParameter1f("alpha", alpha);
			m_possionShader.setParameter1f("rBeta", rBeta);

			m_quad->draw();
			m_possionShader.unuse();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			std::swap(m_p_src, m_p_dst);
		}
	}
}

void Fluid2D::gradientSubtraction() {
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_velocity[m_v_dst], 0);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_velocity[m_v_src]);

		m_boundaryAdvectShader.use();

		m_boundaryAdvectShader.setParameter1f("scale", -1);
		m_boundaryAdvectShader.setParameter1i("field", 0);
		m_boundaryAdvectShader.setParameter1i("textureWidth", m_width);

		m_quad->draw();
		m_boundaryAdvectShader.unuse();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		std::swap(m_v_src, m_v_dst);
	}

	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_velocity[m_v_dst], 0);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_velocity[m_v_src]);

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, m_pressure[m_p_src]);

		m_gradientSubtractionShader.use();

		float halfrdx = 0.5f / m_dx;

		m_gradientSubtractionShader.setParameter1i("w", 0);
		m_gradientSubtractionShader.setParameter1i("p", 1);
		m_gradientSubtractionShader.setParameter1i("textureWidth", m_width);
		m_gradientSubtractionShader.setParameter1f("halfrdx", halfrdx);

		m_quad->draw();
		m_gradientSubtractionShader.unuse();
	
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		std::swap(m_v_src, m_v_dst);
	}
}
