#pragma once
#include "Transformation.hpp"

class Rotate : public Transformation {
 public:
  float angle, x, y, z;
  Rotate(float angle, float x, float y, float z)
      : angle(angle), x(x), y(y), z(z) {}
  glm::mat4 apply(const glm::mat4& matrix) const override {
    return glm::rotate(matrix, glm::radians(angle), glm::vec3(x, y, z));
  }
};