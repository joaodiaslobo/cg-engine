#include <iostream>
#include "settings.hpp"
#include "window/Window.hpp"

int main(const int argc, char *argv[])
{
    DisplaySettings settings;
    settings.width = 800;
    settings.height = 600;
    settings.fullscreen = false;

    if (Window::initialize(&settings) == -1) {
        return -1;
    }

    return 0;
}