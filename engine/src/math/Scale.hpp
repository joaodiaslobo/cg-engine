#pragma once
#include "Transformation.hpp"

class Scale : public Transformation {
 public:
  float x, y, z;
  Scale(float x, float y, float z) : x(x), y(y), z(z) {}
};