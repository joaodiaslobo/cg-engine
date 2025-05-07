#pragma once

class Settings {
 private:
  bool showNormals = false;
  bool showWireframe = true;

 public:
  void toggleNormals();
  void toggleWireframe();
  bool getShowNormals();
  bool getShowWireframe();
};