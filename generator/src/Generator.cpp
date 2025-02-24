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
vec3 polarToCartesian(float radius, float alpha, float y) {
  return vec3(radius * sin(alpha), y, radius * cos(alpha));
}

Model Cone(float radius, float height, int slices, int stacks) {
  vector<vec3> vertices;

  float sliceSize = 2 * M_PI / slices;
  float stackSize = height / stacks;

  // VERTICES

  vec3 baseMiddle = vec3(0, 0, 0);

  for (int slice = 0; slice < slices; slice++) {
    for (int stack = 0; stack < stacks; stack++) {
      float currentRadius = radius - stack * radius / stacks;
      float nextRadius = radius - (stack + 1) * radius / stacks;

      vec3 bottomLeft =
          polarToCartesian(currentRadius, slice * sliceSize, stack * stackSize);
      vec3 bottomRight = polarToCartesian(
          currentRadius, (slice + 1) * sliceSize, stack * stackSize);
      vec3 topLeft = polarToCartesian(nextRadius, slice * sliceSize,
                                      (stack + 1) * stackSize);
      vec3 topRight = polarToCartesian(nextRadius, (slice + 1) * sliceSize,
                                       (stack + 1) * stackSize);

      vertices.insert(vertices.end(), {topLeft, bottomLeft, bottomRight});
      vertices.insert(vertices.end(), {topLeft, bottomRight, topRight});
    }

    vec3 baseBottomLeft = polarToCartesian(radius, slice * sliceSize, 0);
    vec3 baseBottomRight = polarToCartesian(radius, (slice + 1) * sliceSize, 0);

    vertices.insert(vertices.end(),
                    {baseMiddle, baseBottomRight, baseBottomLeft});
  }

  return {vertices};
}

Model Plane(float length, int divisions) {
  vector<vec3> vertices;

  float divisionSize = length / divisions;

  for (int x = 0; x < divisions; x++) {
    for (int z = 0; z < divisions; z++) {
      vec3 bottomLeft = vec3(x * divisionSize, 0, z * divisionSize);
      vec3 bottomRight = vec3((x + 1) * divisionSize, 0, z * divisionSize);
      vec3 topLeft = vec3(x * divisionSize, 0, (z + 1) * divisionSize);
      vec3 topRight = vec3((x + 1) * divisionSize, 0, (z + 1) * divisionSize);

      vertices.insert(vertices.end(), {topLeft, bottomLeft, bottomRight});
      vertices.insert(vertices.end(), {topLeft, bottomRight, topRight});
    }
  }

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
