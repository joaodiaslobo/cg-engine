#pragma once

#include <string>
#include <vector>

#include "Group.hpp"
#include "Light.hpp"

struct Scene {
 private:
  Group root;
  vector<Light> lights;
  float time = 0.0f;

 public:
  void render(bool showNormals);

  void setRoot(Group&& root) { this->root = std::move(root); }

  const Group& getRoot() const { return root; }

  void addLight(const Light& light) { lights.push_back(light); }

  void clear();

  void updateTime(float delta) { time += delta; }

  float getTime() const { return time; }
};