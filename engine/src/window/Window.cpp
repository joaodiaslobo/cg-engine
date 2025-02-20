#include "Window.h"
#include "settings.hpp"
#include <GLFW/glfw3.h>
#include "debug/Logger.hpp"

static debug::Logger logger("window");

int Window::initialize(DisplaySettings *settings) {
    Window::settings = settings;
    Window::width = settings->width;
    Window::height = settings->height;

    if(glfwInit() == GLFW_FALSE) {
        logger.error("Failed to initialize GLFW.");
        return -1;
    }
}