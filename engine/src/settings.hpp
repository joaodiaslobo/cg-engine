#pragma once

struct EngineSettings {
  DisplaySettings display;
};

struct DisplaySettings {
  bool fullscreen = false;
  int width = 1280;
  int height = 720;
  int framerate = 120;
};