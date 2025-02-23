#pragma once
#include "../window/Window.hpp"

class Engine {
 public:
  bool initialize();
  void run();
  void render();

 private:
  Window window;
  Scene scene;
  Camera camera;
};