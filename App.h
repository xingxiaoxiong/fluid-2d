#pragma once

#include "mylib/MinimalOpenGL.h"
#include "Fluid2D.h"

#include <memory>

class App {
	GLFWwindow* m_window = nullptr;
	int m_width, m_height;
	std::unique_ptr<Fluid2D> m_fluid;
	bool m_bLeftMouseButtonPressed;

	void initializeGLFW(std::string name);
	void setup();
	void applyForce(float, float, float, float);
public:
	App(int width, int height, std::string name) {
		m_width = width;
		m_height = height;
		initializeGLFW(name);
		setup();
	}

	void setLeftMouseButtonPress(bool);

	~App();

	void run();
};

