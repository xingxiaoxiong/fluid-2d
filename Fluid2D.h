#pragma once

#include "MinimalOpenGL.h"
#include "Mesh.h"
#include <memory>


class Fluid2D {
	int m_width, m_height;
	float m_viscosity;

	GLuint m_fbo;
	GLuint m_velocity[2], m_pressure[2];
	GLuint m_divergence;
	GLuint m_dye[2];
	GLuint m_v_src, m_v_dst;
	GLuint m_p_src, m_p_dst;
	GLuint m_dye_src, m_dye_dst;

	std::unique_ptr<Mesh> m_line, m_quad;

	GLuint testShader;
	GLuint m_copyShader;
	GLuint m_boundaryAdvectShader;
	GLuint m_interiorAdvectShader;
	GLuint m_applyForceShader;
	GLuint m_possionShader;
	GLuint m_divergenceShader;
	GLuint m_gradientSubtractionShader;

	bool m_bApplyForce;
	float m_forceStartX, m_forceStartY, m_forceX, m_forceY;

	void setup();

	void advectVelocity(float);
	void advect_dye(float);
	void applyForce();
	void diffuseVelocity(float);
	void computePressure();
	void gradientSubtraction();
	void divergence();
	void solvePossion(int);
	void copy(const GLuint* two, unsigned int, unsigned int);

	// for testing purposes
	void testBoundaryDrawing();
	void testBoundaryAdvect();
	void testInteriorAdvect(float);
	void testApplyForce(float, float, float, float);
	void testDye();

public:
	Fluid2D(int width, int height);
	void update(float timeStep);
	void applyForce(float, float, float, float);
};
