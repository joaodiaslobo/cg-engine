#include "Engine.hpp"

#include <GLFW/glfw3.h>

#include "settings.hpp"
#include "window/Window.hpp"

bool Engine::initialize() {
  DisplaySettings settings;
  settings.width = 800;
  settings.height = 600;
  settings.fullscreen = false;

  if (!window.initialize(&settings)) {
    return false;
  }

  return true;
}

void Engine::run() {
  while (!glfwWindowShouldClose(window.getGlfwWindow())) {
    glfwPollEvents();

    render();
    glfwSwapBuffers(window.getGlfwWindow());
  }

  Window::terminate();
}

void renderSceneAxis() {
  glBegin(GL_LINES);

  // x-axis
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(-1000.0, 0.0, 0.0);
  glVertex3f(1000.0, 0.0, 0.0);

  // y-axis
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, -1000.0, 0.0);
  glVertex3f(0.0, 1000.0, 0.0);

  // z-axis
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0.0, 0.0, -1000.0);
  glVertex3f(0.0, 0.0, 1000.0);

  glColor3f(1.0, 1.0, 1.0);

  glEnd();
}

void Engine::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  renderSceneAxis();
}