#pragma once

#include "MinimalOpenGL.h"
#include "Mesh.h"
#include <memory>


class Fluid2D {
	int m_width, m_height;

	GLuint m_fbo;
	GLuint m_velocity[2], m_pressure;
	GLuint m_dye[2];

	std::unique_ptr<Mesh> m_line, m_quad;

	GLuint testShader;
	GLuint m_copyShader;
	GLuint m_boundaryAdvectShader;
	GLuint m_interiorAdvectShader;
	GLuint m_applyForceShader;

	bool m_bApplyForce;
	float m_forceStartX, m_forceStartY, m_forceX, m_forceY;

	void setup();

	void advect(float);
	void applyForce();
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
