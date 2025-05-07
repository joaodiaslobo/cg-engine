#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <tinyxml2.h>

#include <string>

#include "../scene/Group.hpp"
#include "../scene/Scene.hpp"
#include "../ui/UI.hpp"
#include "../window/Camera.hpp"
#include "../window/Window.hpp"
#include "debug/Logger.hpp"
#include "settings.hpp"

class Engine {
 private:
  Window window;
  Scene scene;
  Camera camera;
  UI ui;
  Settings settings;

 public:
  bool initialize();
  bool initializeFromFile(const string& filename);
  bool Engine::loadNewFile(const string& filename);
  void configureGlfw(Window& window);
  void run();
  void render();
  void setupProjectionAndView();
  Window* getWindow() { return &window; }
  Camera* getCamera() { return &camera; }
  Scene* getScene() { return &scene; }
  UI* getUI() { return &ui; }
  Settings* getSettings() { return &settings; }
};

void windowSizeUpdatedCallback(GLFWwindow* window, int width, int height);

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);

void mouseCallback(GLFWwindow* window, double xpos, double ypos);

void glfwErrorCallback(const int error, const char* description);