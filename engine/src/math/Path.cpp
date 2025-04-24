#include "Path.hpp"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <iostream>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

glm::mat4 Path::apply(const glm::mat4& matrix, float time) const {
  vec3 derivative;
  vec3 position = GetPathPosition(time / duration, derivative);
  glm::mat4 result = glm::translate(glm::mat4(1.0f), position);

  if (align) {
    vec3 x_axis = glm::normalize(derivative);
    vec3 z_axis = glm::normalize(glm::cross(x_axis, last_y_axis));
    vec3 y_axis = glm::normalize(glm::cross(z_axis, x_axis));

    last_y_axis = y_axis;

    glm::mat4 rotation_matrix =
        glm::mat4(glm::vec4(x_axis, 0.0f), glm::vec4(y_axis, 0.0f),
                  glm::vec4(z_axis, 0.0f), glm::vec4(0, 0, 0, 1));

    result *= rotation_matrix;
  }

  if (render_path) {
    glBegin(GL_LINE_LOOP);

    const size_t segments = 100;

    for (int i = 0; i < segments; ++i) {
      const float time = static_cast<float>(i) / static_cast<float>(segments);
      vec3 derivative;
      vec3 position = GetPathPosition(time, derivative);
      glVertex3f(position.x, position.y, position.z);
    }
    glEnd();
  }

  return result * matrix;
}

glm::vec3 Path::GetPathPosition(float time, vec3& derivative) const {
  glm::vec3 position;

  int point_count = path_points.size();
  float t = time * point_count;
  int i = floor(t);
  float time_in_segment = t - i;

  const glm::vec3& p0 = path_points[(i + point_count - 1) % point_count];
  const glm::vec3& p1 = path_points[i % point_count];
  const glm::vec3& p2 = path_points[(i + 1) % point_count];
  const glm::vec3& p3 = path_points[(i + 2) % point_count];

  glm::mat4 catmull_matrix = glm::transpose(
      glm::mat4(-0.5f, 1.5f, -1.5f, 0.5f, 1.0f, -2.5f, 2.0f, -0.5f, -0.5f, 0.0f,
                0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f));

  glm::vec4 time_vector =
      glm::vec4(time_in_segment * time_in_segment * time_in_segment,
                time_in_segment * time_in_segment, time_in_segment, 1.0f);
  glm::vec4 time_derivative = glm::vec4(3 * time_in_segment * time_in_segment,
                                        2 * time_in_segment, 1.0f, 0.0f);

  for (int j = 0; j < 3; j++) {
    glm::vec4 p = glm::vec4(p0[j], p1[j], p2[j], p3[j]);
    glm::vec4 result = catmull_matrix * p;

    position[j] = glm::dot(result, time_vector);
    derivative[j] = glm::dot(result, time_derivative);
  }

  return position;
}