#pragma once

#include <string>
#include <vector>

#include "Group.hpp"

struct Scene {
 private:
  Group root;
  float time = 0.0f;

 public:
  void render();

  void setRoot(Group&& root) { this->root = std::move(root); }

  const Group& getRoot() const { return root; }

  void clear();

  void updateTime(float delta) { time += delta; }
  float getTime() const { return time; }
};