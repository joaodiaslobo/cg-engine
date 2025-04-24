#pragma once
#include <glm/glm.hpp>

#include "Transformation.hpp"

using glm::vec3;
using glm::vec4;

class Path : public Transformation {
 private:
  vec3 GetPathPosition(float time, vec3& derivative) const;

 public:
  float duration;
  bool align;
  std::vector<vec3> path_points;
  bool render_path;
  mutable vec3 last_y_axis = glm::vec3(0, 1, 0);

  Path(float duration, bool align, const std::vector<vec3>& path_points,
       bool render_path)
      : duration(duration),
        align(align),
        path_points(path_points),
        render_path(render_path) {}
  glm::mat4 apply(const glm::mat4& matrix, float time) const override;
};