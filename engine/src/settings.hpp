#pragma once

struct DisplaySettings {
  bool fullscreen = false;
  int width = 1280;
  int height = 720;
  int framerate = 120;
};

struct EngineSettings {
  DisplaySettings *display;
};