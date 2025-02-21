#include "Window.hpp"
#include "../settings.hpp"
#include <GLFW/glfw3.h>
#include "../debug/Logger.hpp"

static debug::Logger logger;

int Window::initialize(DisplaySettings *settings) {
    Window::settings = settings;
    Window::width = settings->width;
    Window::height = settings->height;

    if(glfwInit() == GLFW_FALSE) {
        logger.error("Failed to initialize GLFW.");
        return -1;
    }

    window = glfwCreateWindow(width, height, "CG@DI", nullptr, nullptr);

    if (window == nullptr) {
        logger.error("Failed to create GLFW window.");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
}

void Window::setFramerate(int framerate) {
    if ((framerate != -1) != (Window::framerate != -1)) {
        glfwSwapInterval(framerate == -1);
    }
    Window::framerate = framerate;
}
