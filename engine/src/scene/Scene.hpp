#pragma once

#include <string>
#include <vector>

#include "Group.hpp"

struct Scene {
 private:
  Group root;

 public:
  void render();
  void setRoot(Group root) { this->root = root; }
};