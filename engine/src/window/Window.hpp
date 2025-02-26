#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "../settings.hpp"

class Window {
  static GLFWwindow* glfwWindow;
  static DisplaySettings* settings;
  static bool fullscreen;
  static int framerate;

 public:
  static int posX;
  static int posY;
  static unsigned int width;
  static unsigned int height;
  static bool initialize(DisplaySettings* settings, const char* title);
  static void terminate();

  static void setWindowSize(unsigned int width, unsigned int height);

  static glm::vec2 size() { return glm::vec2(width, height); }

  static void setFramerate(int framerate);

  static GLFWwindow* getGlfwWindow();
};