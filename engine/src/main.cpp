#include <iostream>

#include "engine/Engine.hpp"
#include "settings.hpp"

int main(const int argc, char *argv[]) {
  Engine engine;
  if (!engine.initialize()) {
    return -1;
  }

  engine.run();

  return 0;
}