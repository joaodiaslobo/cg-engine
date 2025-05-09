#pragma once

enum ViewMode { WIREFRAME, FLAT, SHADED };

class Settings {
 private:
  bool showNormals = false;
  ViewMode viewMode = WIREFRAME;

 public:
  void toggleNormals();
  void toggleViewmode();
  bool getShowNormals();
  ViewMode getViewmode();
};