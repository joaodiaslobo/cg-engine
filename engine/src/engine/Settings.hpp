#pragma once

enum ViewMode { WIREFRAME, FLAT, SHADED };

class Settings {
 private:
 public:
  ViewMode viewMode = SHADED;
  bool showNormals = false;
  bool showAxis = true;
  bool isPaused = false;
  bool getShowAxis();
  void toggleNormals();
  void toggleViewmode();
  bool getShowNormals();
  bool getPaused();
  ViewMode getViewmode();
};