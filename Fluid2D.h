#pragma once

#include "mylib/MinimalOpenGL.h"
#include "mylib/Mesh.h"
#include "mylib/Shader.h"
#include <memory>


class Fluid2D {
	int m_width, m_height;
	float m_viscosity;

	float m_dx; // grid scale

	GLuint m_fbo;
	GLuint m_velocity[2], m_pressure[2];
	GLuint m_divergence;
	GLuint m_dye[2];
	GLuint m_v_src, m_v_dst;
	GLuint m_p_src, m_p_dst;
	GLuint m_dye_src, m_dye_dst;

	std::unique_ptr<Mesh> m_line, m_quad;

	ShaderProgram testShader;
	ShaderProgram m_boundaryAdvectShader;
	ShaderProgram m_interiorAdvectShader;
	ShaderProgram m_applyForceShader;
	ShaderProgram m_possionShader;
	ShaderProgram m_divergenceShader;
	ShaderProgram m_gradientSubtractionShader;

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

public:
	Fluid2D(int width, int height);
	void update(float timeStep);
	void applyForce(float, float, float, float);
};
