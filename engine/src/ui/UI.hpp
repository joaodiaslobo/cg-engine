#pragma once

#include <GLFW/glfw3.h>

#include "../window/Window.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class UI {
 private:
  Window *window;
  float fps = 0.0f;
  ImGuiIO *io = nullptr;

 public:
  void initialize(Window *window);
  void render();
  void postRender();
  void shutdown();
  void setFPS(float fps) { this->fps = fps; }
};