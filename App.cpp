

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
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			auto app = (App*)glfwGetWindowUserPointer(pWindow);
			if (GLFW_PRESS == action)
				app->setLeftMouseButtonPress(true);
			else if (GLFW_RELEASE == action)
				app->setLeftMouseButtonPress(false);
		}
		
	});
}

void App::setup() {
	m_fluid = std::make_unique<Fluid2D>(m_width, m_height);
}

void App::setLeftMouseButtonPress(bool pressed) {
	m_bLeftMouseButtonPressed = pressed;
}

void App::run() {

	glViewport(0, 0, m_width, m_height);
	glEnable(GL_DEPTH_TEST);

	auto prevTime = glfwGetTime();
	auto currentTime = glfwGetTime();

	double mouseStartX = -1, mouseStartY = -1;
	double mouseEndX, mouseEndY;

	while (!glfwWindowShouldClose(m_window))
	{
		glClearColor(0.1, 0.1, 0.1, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		currentTime = glfwGetTime();
		m_fluid->update(currentTime - prevTime);
		
		if (m_bLeftMouseButtonPressed) {
			if (mouseStartX < 0) {
				glfwGetCursorPos(m_window, &mouseStartX, &mouseStartY);
			}
			else {
				glfwGetCursorPos(m_window, &mouseEndX, &mouseEndY);
				float deltaX = mouseEndX - mouseStartX;
				float deltaY = mouseEndY - mouseStartY;
				applyForce(mouseStartX, mouseStartY, deltaX, deltaY);
				mouseStartX = mouseEndX;
				mouseStartY = mouseEndY;
			}
		}
		else {
			mouseStartX = -1;
			mouseStartY = -1;
		}

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