#pragma once

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <tinyxml2.h>

#include "../scene/Scene.hpp"
#include "../window/Camera.hpp"
#include "../window/Window.hpp"
#include "debug/Logger.hpp"
#include "settings.hpp"

class Engine {
 public:
  bool initialize();
  bool initializeFromFile(const string& filename);
  void run();
  void render();
  void updateViewport();

 private:
  Window window;
  Scene scene;
  Camera camera;
};