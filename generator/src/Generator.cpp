#include "Generator.hpp"

#include <glm/glm.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include <fstream>
#include <iostream>

using glm::vec3;
using std::vector;

namespace generator {
Model Cone(float radius, float height, int slices, int stacks) {
  vector<vec3> vertices;

  float slice_size = 2 * M_PI / slices;
  float stack_size = height / stacks;

  // VERTICES

  // Base center
  vertices.push_back(vec3(0.0f, 0.0f, 0.0f));
  int base_center_index = 0;

  // Base circle vertices
  for (int i = 0; i < slices; i++) {
    float theta = i * slice_size;
    float x = radius * cos(theta);
    float z = radius * sin(theta);
    vertices.push_back(vec3(x, 0.0f, z));
  }

  // Side surface vertices
  int base_vertices_start = 1;
  int side_vertices_start = base_vertices_start + slices;
  for (int i = 1; i <= stacks; i++) {
    float y = i * stack_size;
    float current_radius = radius * (1.0f - (float)i / stacks);

    for (int j = 0; j < slices; j++) {
      float theta = j * slice_size;
      float x = current_radius * cos(theta);
      float z = current_radius * sin(theta);
      vertices.push_back(vec3(x, y, z));
    }
  }

  // Tip of the cone
  int tip_index = vertices.size();
  vertices.push_back(vec3(0.0f, height, 0.0f));

  return {vertices};
}

bool Export(const Model& model, const std::string& filename) {
  std::ofstream file(filename);
  if (!file.is_open()) {
    return false;
  }

  for (const auto& vertex : model.vertices) {
    file << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
  }

  file.flush();

  return true;
}
}  // namespace generator
