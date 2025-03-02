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

vec3 sphericalToCartesian(float radius, float alpha, float beta) {
  return vec3(radius * cos(beta) * sin(alpha), radius * sin(beta),
              radius * cos(beta) * cos(alpha));
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

Model Sphere(float radius, int slices, int stacks) {
  vector<vec3> vertices;

  float sliceSize = 2 * M_PI / slices;
  float stackSize = M_PI / stacks;

  // VERTICES

  for (int slice = 0; slice < slices; slice++) {
    for (int stack = 0; stack < stacks; stack++) {
      vec3 bottomLeft = sphericalToCartesian(radius, slice * sliceSize,
                                             stack * stackSize - M_PI_2);
      vec3 bottomRight = sphericalToCartesian(radius, (slice + 1) * sliceSize,
                                              stack * stackSize - M_PI_2);
      vec3 topLeft = sphericalToCartesian(radius, slice * sliceSize,
                                          (stack + 1) * stackSize - M_PI_2);
      vec3 topRight = sphericalToCartesian(radius, (slice + 1) * sliceSize,
                                           (stack + 1) * stackSize - M_PI_2);

      vertices.insert(vertices.end(), {topLeft, bottomLeft, bottomRight});
      vertices.insert(vertices.end(), {topLeft, bottomRight, topRight});
    }
  }
  return {vertices};
}

Model Plane(float length, int divisions) {
  vector<vec3> vertices;

  float divisionSize = length / divisions;
  float halfLength = length / 2.0f;

  // VERTICES

  for (int x = 0; x < divisions; x++) {
    for (int z = 0; z < divisions; z++) {
      float xPos = (x * divisionSize) - halfLength;
      float zPos = (z * divisionSize) - halfLength;
      float xNext = ((x + 1) * divisionSize) - halfLength;
      float zNext = ((z + 1) * divisionSize) - halfLength;

      vec3 bottomLeft = vec3(xPos, 0, zPos);
      vec3 bottomRight = vec3(xNext, 0, zPos);
      vec3 topLeft = vec3(xPos, 0, zNext);
      vec3 topRight = vec3(xNext, 0, zNext);

      vertices.insert(vertices.end(), {topLeft, bottomLeft, bottomRight});
      vertices.insert(vertices.end(), {topLeft, bottomRight, topRight});

      vertices.insert(vertices.end(), {bottomRight, bottomLeft, topLeft});
      vertices.insert(vertices.end(), {topRight, bottomRight, topLeft});
    }
  }
  return {vertices};
}

Model Box(float size, int divisions) {
  float halfSize = size / 2.0f;
  float step = size / divisions;
  vector<vec3> vertices;

  // VERTICES

  for (int i = 0; i < divisions; ++i) {
    for (int j = 0; j < divisions; ++j) {
      float v1 = -halfSize + i * step;
      float u1 = -halfSize + j * step;
      float v2 = v1 + step;
      float u2 = u1 + step;

      vertices.insert(vertices.end(),
                      {vec3(v1, u1, halfSize), vec3(v2, u1, halfSize),
                       vec3(v1, u2, halfSize)});
      vertices.insert(vertices.end(),
                      {vec3(v1, u2, halfSize), vec3(v2, u1, halfSize),
                       vec3(v2, u2, halfSize)});

      vertices.insert(vertices.end(),
                      {vec3(v1, u1, -halfSize), vec3(v1, u2, -halfSize),
                       vec3(v2, u1, -halfSize)});
      vertices.insert(vertices.end(),
                      {vec3(v1, u2, -halfSize), vec3(v2, u2, -halfSize),
                       vec3(v2, u1, -halfSize)});

      vertices.insert(vertices.end(),
                      {vec3(-halfSize, v1, u1), vec3(-halfSize, v1, u2),
                       vec3(-halfSize, v2, u1)});
      vertices.insert(vertices.end(),
                      {vec3(-halfSize, v1, u2), vec3(-halfSize, v2, u2),
                       vec3(-halfSize, v2, u1)});

      vertices.insert(vertices.end(),
                      {vec3(halfSize, v1, u1), vec3(halfSize, v2, u1),
                       vec3(halfSize, v1, u2)});
      vertices.insert(vertices.end(),
                      {vec3(halfSize, v1, u2), vec3(halfSize, v2, u1),
                       vec3(halfSize, v2, u2)});

      vertices.insert(vertices.end(),
                      {vec3(v1, halfSize, u1), vec3(v1, halfSize, u2),
                       vec3(v2, halfSize, u1)});
      vertices.insert(vertices.end(),
                      {vec3(v1, halfSize, u2), vec3(v2, halfSize, u2),
                       vec3(v2, halfSize, u1)});

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
  float halfHeight = height / 2.0f;

  vec3 baseMiddle = vec3(0, -halfHeight, 0);
  vec3 topMiddle = vec3(0, halfHeight, 0);

  // SIDE FACES
  for (int slice = 0; slice < slices; slice++) {
    float angle1 = slice * sliceSize;
    float angle2 = (slice + 1) * sliceSize;
    float stackSize = height / stacks;

    for (int stack = 0; stack < stacks; stack++) {
      float currentHeight = stack * stackSize - halfHeight;
      float nextHeight = (stack + 1) * stackSize - halfHeight;

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

    vec3 baseLeft = polarToCartesian(radius, angle1, -halfHeight);
    vec3 baseRight = polarToCartesian(radius, angle2, -halfHeight);
    vec3 topLeft = polarToCartesian(radius, angle1, halfHeight);
    vec3 topRight = polarToCartesian(radius, angle2, halfHeight);

    vertices.insert(vertices.end(), {baseMiddle, baseRight, baseLeft});
    vertices.insert(vertices.end(), {topMiddle, topLeft, topRight});
  }

  return {vertices};
}

Model Torus(float radius, float tubeRadius, int slices, int stacks) {
  vector<vec3> vertices;

  for (int stack = 0; stack < stacks; stack++) {
    float theta1 = 2 * stack * M_PI / stacks;
    float theta2 = 2 * (stack + 1) * M_PI / stacks;

    for (int slice = 0; slice < slices; slice++) {
      float phi1 = 2 * slice * M_PI / slices;
      float phi2 = 2 * (slice + 1) * M_PI / slices;

      vec3 topLeft = vec3((radius + tubeRadius * cos(phi1)) * cos(theta1),
                          tubeRadius * sin(phi1),
                          (radius + tubeRadius * cos(phi1)) * sin(theta1));
      vec3 topRight = vec3((radius + tubeRadius * cos(phi1)) * cos(theta2),
                           tubeRadius * sin(phi1),
                           (radius + tubeRadius * cos(phi1)) * sin(theta2));
      vec3 bottomLeft = vec3((radius + tubeRadius * cos(phi2)) * cos(theta1),
                             tubeRadius * sin(phi2),
                             (radius + tubeRadius * cos(phi2)) * sin(theta1));
      vec3 bottomRight = vec3((radius + tubeRadius * cos(phi2)) * cos(theta2),
                              tubeRadius * sin(phi2),
                              (radius + tubeRadius * cos(phi2)) * sin(theta2));

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
