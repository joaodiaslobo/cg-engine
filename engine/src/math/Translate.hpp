#pragma once
#include "Transformation.hpp"

class Translate : public Transformation {
 public:
  float x, y, z;
  Translate(float x, float y, float z) : x(x), y(y), z(z) {}
};