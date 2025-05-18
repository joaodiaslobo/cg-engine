#include "Settings.hpp"

#include "debug/Logger.hpp"

static debug::Logger logger;

bool Settings::getShowNormals() { return showNormals; }

void Settings::toggleViewmode() {
  switch (viewMode) {
    case WIREFRAME:
      viewMode = FLAT;
      break;
    case FLAT:
      viewMode = SHADED;
      break;
    case SHADED:
      viewMode = WIREFRAME;
      break;
  }
}

void Settings::toggleNormals() { showNormals = !showNormals; }

ViewMode Settings::getViewmode() { return viewMode; }

bool Settings::getShowAxis() { return showAxis; }

bool Settings::getPaused() { return isPaused; }