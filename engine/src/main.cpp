#include <iostream>

#include "engine/Engine.hpp"
#include "settings.hpp"

int main(const int argc, char *argv[]) {
  Engine engine;

  if(argc == 1) {    
    if (!engine.initialize()) {
      return -1;
    }
  } else {
    
  }

  engine.run();

  return 0;
}