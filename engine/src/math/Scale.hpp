#pragma once
#include "Transformation.hpp"

class Scale : public Transformation {
 public:
  float x, y, z;
  Scale(float x, float y, float z) : x(x), y(y), z(z) {}
  glm::mat4 apply(const glm::mat4& matrix) const override {
    return glm::scale(matrix, glm::vec3(x, y, z));
  }
};