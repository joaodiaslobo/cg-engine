#include <iostream>

#include "debug/Logger.hpp"
#include "engine/Engine.hpp"
#include "settings.hpp"

static debug::Logger logger;

int main(const int argc, char *argv[]) {
  Engine engine;

  if (argc == 1) {
    logger.info("Loading default empty scene.");
    if (!engine.initialize()) {
      return -1;
    }
  } else {
    logger.info("Loading scene from file: " + string(argv[1]) + ".");
    if (!engine.initializeFromFile(argv[1])) {
      return -1;
    }
  }

  engine.run();

  return 0;
}