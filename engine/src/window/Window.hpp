#pragma once

#include "../settings.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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
    static bool initialize(DisplaySettings* settings);
    static void terminate();

    static DisplaySettings* getSettings();

    static glm::vec2 size() {
        return glm::vec2(width, height);
    }

    static void setFramerate(int framerate);

    static GLFWwindow* getGlfwWindow();
};