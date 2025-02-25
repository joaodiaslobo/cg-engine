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
  float halfLength = length / 2.0f;

  for (int x = 0; x < divisions; x++) {
    for (int z = 0; z < divisions; z++) {
      // Offset x and z so that the plane is centered at the origin
      float xPos = (x * divisionSize) - halfLength;
      float zPos = (z * divisionSize) - halfLength;
      float xNext = ((x + 1) * divisionSize) - halfLength;
      float zNext = ((z + 1) * divisionSize) - halfLength;

      vec3 bottomLeft = vec3(xPos, 0, zPos);
      vec3 bottomRight = vec3(xNext, 0, zPos);
      vec3 topLeft = vec3(xPos, 0, zNext);
      vec3 topRight = vec3(xNext, 0, zNext);

      // triangles
      vertices.insert(vertices.end(), {topLeft, bottomLeft, bottomRight});
      vertices.insert(vertices.end(), {topLeft, bottomRight, topRight});
    }
  }
  return {vertices};
}

Model Box(float size, int divisions) {
  float halfSize = size / 2.0f;
  float step = size / divisions;
  vector<vec3> vertices;

  for (int i = 0; i < divisions; ++i) {
    for (int j = 0; j < divisions; ++j) {
      float v1 = -halfSize + i * step;
      float u1 = -halfSize + j * step;
      float v2 = v1 + step;
      float u2 = u1 + step;

      // Face Frontal
      vertices.insert(vertices.end(),
                      {vec3(v1, u1, halfSize), vec3(v2, u1, halfSize),
                       vec3(v1, u2, halfSize)});
      vertices.insert(vertices.end(),
                      {vec3(v1, u2, halfSize), vec3(v2, u1, halfSize),
                       vec3(v2, u2, halfSize)});

      // Face Traseira
      vertices.insert(vertices.end(),
                      {vec3(v1, u1, -halfSize), vec3(v1, u2, -halfSize),
                       vec3(v2, u1, -halfSize)});
      vertices.insert(vertices.end(),
                      {vec3(v1, u2, -halfSize), vec3(v2, u2, -halfSize),
                       vec3(v2, u1, -halfSize)});

      // Face Esquerda
      vertices.insert(vertices.end(),
                      {vec3(-halfSize, v1, u1), vec3(-halfSize, v1, u2),
                       vec3(-halfSize, v2, u1)});
      vertices.insert(vertices.end(),
                      {vec3(-halfSize, v1, u2), vec3(-halfSize, v2, u2),
                       vec3(-halfSize, v2, u1)});

      // Face Direita
      vertices.insert(vertices.end(),
                      {vec3(halfSize, v1, u1), vec3(halfSize, v2, u1),
                       vec3(halfSize, v1, u2)});
      vertices.insert(vertices.end(),
                      {vec3(halfSize, v1, u2), vec3(halfSize, v2, u1),
                       vec3(halfSize, v2, u2)});

      // Face Superior
      vertices.insert(vertices.end(),
                      {vec3(v1, halfSize, u1), vec3(v1, halfSize, u2),
                       vec3(v2, halfSize, u1)});
      vertices.insert(vertices.end(),
                      {vec3(v1, halfSize, u2), vec3(v2, halfSize, u2),
                       vec3(v2, halfSize, u1)});

      // Face Inferior
      vertices.insert(vertices.end(),
                      {vec3(v1, -halfSize, u1), vec3(v2, -halfSize, u1),
                       vec3(v1, -halfSize, u2)});
      vertices.insert(vertices.end(),
                      {vec3(v1, -halfSize, u2), vec3(v2, -halfSize, u1),
                       vec3(v2, -halfSize, u2)});
    }
  }

  return Model{vertices};
}

Model Cylinder(float radius, float height, int slices, int stacks) {
  vector<vec3> vertices;

  float sliceSize = 2 * M_PI / slices;
  float stackSize = height / stacks;

  vec3 baseMiddle = vec3(0, 0, 0);
  vec3 topMiddle = vec3(0, height, 0);

  // SIDE FACES
  for (int slice = 0; slice < slices; slice++) {
    float angle1 = slice * sliceSize;
    float angle2 = (slice + 1) * sliceSize;

    for (int stack = 0; stack < stacks; stack++) {
      float currentHeight = stack * stackSize;
      float nextHeight = (stack + 1) * stackSize;

      vec3 bottomLeft = polarToCartesian(radius, angle1, currentHeight);
      vec3 bottomRight = polarToCartesian(radius, angle2, currentHeight);
      vec3 topLeft = polarToCartesian(radius, angle1, nextHeight);
      vec3 topRight = polarToCartesian(radius, angle2, nextHeight);

      vertices.insert(vertices.end(), {bottomLeft, bottomRight, topLeft});
      vertices.insert(vertices.end(), {topLeft, bottomRight, topRight});
    }
  }

  // BASES
  for (int slice = 0; slice < slices; slice++) {
    float angle1 = slice * sliceSize;
    float angle2 = (slice + 1) * sliceSize;

    vec3 baseBottomLeft = polarToCartesian(radius, angle1, 0);
    vec3 baseBottomRight = polarToCartesian(radius, angle2, 0);
    vec3 topBaseLeft = polarToCartesian(radius, angle1, height);
    vec3 topBaseRight = polarToCartesian(radius, angle2, height);

    vertices.insert(vertices.end(),
                    {baseMiddle, baseBottomRight, baseBottomLeft});
    vertices.insert(vertices.end(), {topMiddle, topBaseLeft, topBaseRight});
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
