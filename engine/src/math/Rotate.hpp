#pragma once
#include "Transformation.hpp"

class Rotate : public Transformation {
 public:
  float angle, x, y, z;
  Rotate(float angle, float x, float y, float z)
      : angle(angle), x(x), y(y), z(z) {}
};