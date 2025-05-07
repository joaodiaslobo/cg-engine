#include "Settings.hpp"

#include "debug/Logger.hpp"

static debug::Logger logger;

bool Settings::getShowNormals() { return showNormals; }

bool Settings::getShowWireframe() { return showWireframe; }

void Settings::toggleNormals() { showNormals = !showNormals; }

void Settings::toggleWireframe() { showWireframe = !showWireframe; }