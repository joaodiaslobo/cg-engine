#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Transformation {
 public:
  virtual glm::mat4 apply(const glm::mat4& matrix, float time) const = 0;
  virtual ~Transformation() = default;
};