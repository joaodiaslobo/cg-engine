#include "Window.hpp"

#include <GLFW/glfw3.h>

#include "debug/Logger.hpp"
#include "settings.hpp"

static debug::Logger logger;
GLFWwindow* Window::glfwWindow = nullptr;
DisplaySettings* Window::settings = nullptr;
int Window::framerate = 0;
unsigned int Window::width = 0;
unsigned int Window::height = 0;

bool Window::initialize(DisplaySettings* settings, const char* title) {
  Window::settings = settings;
  Window::width = settings->width;
  Window::height = settings->height;

  if (glfwInit() == GLFW_FALSE) {
    logger.error("Failed to initialize GLFW.");
    return false;
  }

  glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

  if (glfwWindow == nullptr) {
    logger.error("Failed to create GLFW window.");
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(glfwWindow);

  return true;
}

void Window::setFramerate(int framerate) {
  if ((framerate != -1) != (Window::framerate != -1)) {
    glfwSwapInterval(framerate == -1);
  }
  Window::framerate = framerate;
}

GLFWwindow* Window::getGlfwWindow() { return glfwWindow; }

void Window::terminate() {
  glfwDestroyWindow(glfwWindow);
  glfwTerminate();
}

void Window::setWindowSize(unsigned int width, unsigned int height) {
  Window::width = width;
  Window::height = height;
}