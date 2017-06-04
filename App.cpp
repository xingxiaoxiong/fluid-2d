

#include "App.h"
#include <iostream>

void App::initializeGLFW(std::string name) {
	if (!glfwInit())
		exit(EXIT_FAILURE);

	m_window = glfwCreateWindow(m_width, m_height, name.c_str(), NULL, NULL);
	if (!m_window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(m_window);

	GLint GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult)
	{
		printf("ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	glfwSetWindowUserPointer(m_window, this);

	// initialize callbacks
	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* pWindow, int button, int action, int mode) {
		static bool lbutton_down = false;
		static double startX, startY, endX, endY;
		static float deltaX, deltaY;
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (GLFW_PRESS == action)
				lbutton_down = true;
			else if (GLFW_RELEASE == action)
				lbutton_down = false;
		}

		if (lbutton_down) {
			glfwGetCursorPos(pWindow, &startX, &startY);
		}
		else {
			glfwGetCursorPos(pWindow, &endX, &endY);
			deltaX = endX - startX;
			deltaY = endY - startY;
			auto app = (App*)glfwGetWindowUserPointer(pWindow);
			app->applyForce(startX, startY, deltaX, deltaY);
		}
		
	});
}

void App::setup() {
	m_fluid = std::make_unique<Fluid2D>(m_width, m_height);
}

void App::run() {

	glViewport(0, 0, m_width, m_height);
	glEnable(GL_DEPTH_TEST);

	auto prevTime = glfwGetTime();
	auto currentTime = glfwGetTime();

	while (!glfwWindowShouldClose(m_window))
	{
		glClearColor(0.1, 0.1, 0.1, 0);
		//glClearColor(1.0, 1.0, 1.0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		currentTime = glfwGetTime();
		m_fluid->update(currentTime - prevTime);
		prevTime = currentTime;

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void App::applyForce(float startX, float startY, float deltaX, float deltaY) {
	m_fluid->applyForce(startX, startY, deltaX, deltaY);
}

App::~App() {

}