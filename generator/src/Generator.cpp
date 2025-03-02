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

/**
 * Generates a 3D model of a cone.
 *
 * @param radius The radius of the base of the cone.
 * @param height The height of the cone.
 * @param slices The number of slices (subdivisions around the cone's
 * circumference).
 * @param stacks The number of stacks (subdivisions along the cone's height).
 * @return A Model object containing the vertices of the cone.
 */
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

/**
 * Generates a 3D model of a sphere.
 *
 * @param radius The radius of the sphere.
 * @param slices The number of vertical slices (longitude lines) of the sphere.
 * @param stacks The number of horizontal stacks (latitude lines) of the sphere.
 * @return A Model object containing the vertices of the generated sphere.
 */
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

/**
 * Generates a 3D model of a plane.
 *
 * @param length The length of the plane.
 * @param divisions The number of divisions along each axis.
 * @return A Model object containing the vertices of the generated plane.
 */
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

/**
 * @brief Generates a 3D box model.
 *
 * @param size The size of the box along each axis.
 * @param divisions The number of divisions along each axis.
 * @return Model The generated 3D box model containing the vertices.
 */
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

/**
 * Generates a 3D model of a cylinder.
 *
 * @param radius The radius of the cylinder.
 * @param height The height of the cylinder.
 * @param slices The number of slices (subdivisions around the cylinder's
 * circumference).
 * @param stacks The number of stacks (subdivisions along the cylinder's
 * height).
 * @return A Model object containing the vertices of the cylinder.
 */
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

/**
 * Generates a 3D torus model.
 *
 * @param radius The radius from the center of the torus to the center of the
 * tube.
 * @param tubeRadius The radius of the tube.
 * @param slices The number of subdivisions around the tube.
 * @param stacks The number of subdivisions around the torus.
 * @return A Model object containing the vertices of the torus.
 */
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

/**
 * @brief Generates an icosphere 3D model.
 *
 * @param radius The radius of the icosphere.
 * @param subdivisions The number of subdivisions to perform on the icosahedron
 * faces. Must be at least 1.
 * @return A Model object containing the vertices of the generated icosphere.
 */
Model Icosphere(float radius, int subdivisions) {
  if (subdivisions < 1) {
    return {};
  }

  vector<vec3> vertices;

  float t = (1.0f + sqrt(5.0f)) / 2.0f;

  vector<vec3> icosahedronVertices = {
      glm::normalize(vec3(-1, t, 0)),  glm::normalize(vec3(1, t, 0)),
      glm::normalize(vec3(-1, -t, 0)), glm::normalize(vec3(1, -t, 0)),
      glm::normalize(vec3(0, -1, t)),  glm::normalize(vec3(0, 1, t)),
      glm::normalize(vec3(0, -1, -t)), glm::normalize(vec3(0, 1, -t)),
      glm::normalize(vec3(t, 0, -1)),  glm::normalize(vec3(t, 0, 1)),
      glm::normalize(vec3(-t, 0, -1)), glm::normalize(vec3(-t, 0, 1))};

  vector<vec3> icosahedronFaces = {
      vec3(0, 11, 5),  vec3(0, 5, 1),  vec3(0, 1, 7),  vec3(0, 7, 10),
      vec3(0, 10, 11), vec3(1, 5, 9),  vec3(5, 11, 4), vec3(11, 10, 2),
      vec3(10, 7, 6),  vec3(7, 1, 8),  vec3(3, 9, 4),  vec3(3, 4, 2),
      vec3(3, 2, 6),   vec3(3, 6, 8),  vec3(3, 8, 9),  vec3(4, 9, 5),
      vec3(2, 4, 11),  vec3(6, 2, 10), vec3(8, 6, 7),  vec3(9, 8, 1)};

  if (subdivisions != 1) {
    for (int i = 0; i < subdivisions - 1; i++) {
      vector<vec3> newVertices;
      vector<vec3> newFaces;

      for (const auto& face : icosahedronFaces) {
        vec3 a = icosahedronVertices[face.x];
        vec3 b = icosahedronVertices[face.y];
        vec3 c = icosahedronVertices[face.z];

        vec3 ab = glm::normalize((a + b) / 2.0f);
        vec3 bc = glm::normalize((b + c) / 2.0f);
        vec3 ca = glm::normalize((c + a) / 2.0f);

        int indexA = newVertices.size();
        newVertices.push_back(a);
        int indexB = newVertices.size();
        newVertices.push_back(b);
        int indexC = newVertices.size();
        newVertices.push_back(c);
        int indexAB = newVertices.size();
        newVertices.push_back(ab);
        int indexBC = newVertices.size();
        newVertices.push_back(bc);
        int indexCA = newVertices.size();
        newVertices.push_back(ca);

        newFaces.push_back(vec3(indexA, indexAB, indexCA));
        newFaces.push_back(vec3(indexAB, indexB, indexBC));
        newFaces.push_back(vec3(indexBC, indexC, indexCA));
        newFaces.push_back(vec3(indexAB, indexBC, indexCA));
      }

      icosahedronVertices = newVertices;
      icosahedronFaces = newFaces;
    }

  } else {
    vector<vec3> newVertices;

    for (const auto& face : icosahedronFaces) {
      vec3 a = icosahedronVertices[face.x];
      vec3 b = icosahedronVertices[face.y];
      vec3 c = icosahedronVertices[face.z];

      vertices.push_back(a * radius);
      vertices.push_back(b * radius);
      vertices.push_back(c * radius);
    }

    icosahedronVertices = newVertices;
  }

  for (const auto& vertex : icosahedronVertices) {
    vertices.push_back(vertex * radius);
  }

  return {vertices};
}

/**
 * @brief Exports the given model to a file.
 *
 * This function writes the vertices of the model to a file specified by the
 * filename. Each vertex is written in the format "v x y z" where x, y, and z
 * are the coordinates of the vertex.
 *
 * @param model The model to be exported.
 * @param filename The name of the file to which the model will be exported.
 * @return true if the file was successfully opened and written to, false
 * otherwise.
 */
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
