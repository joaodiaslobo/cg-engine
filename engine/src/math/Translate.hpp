#pragma once
#include "Transformation.hpp"

class Translate : public Transformation {
 public:
  float x, y, z;
  Translate(float x, float y, float z) : x(x), y(y), z(z) {}

  glm::mat4 apply(const glm::mat4& matrix) const override {
    return glm::translate(matrix, glm::vec3(x, y, z));
  }
};