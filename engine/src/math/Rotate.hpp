#pragma once
#include "Transformation.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

class Rotate : public Transformation {
 public:
  float angle, duration, x, y, z;
  Rotate(float angle, float duration, float x, float y, float z)
      : angle(angle), duration(duration), x(x), y(y), z(z) {}
  glm::mat4 apply(const glm::mat4& matrix, float time) const override {
    if (duration != 0.0f) {
      // Rotation over time
      float angle = static_cast<float>(time * M_PI * 2 / duration);
      return glm::rotate(matrix, angle, glm::vec3(x, y, z));
    }
    // Static rotation
    return glm::rotate(matrix, glm::radians(angle), glm::vec3(x, y, z));
  }
};