#include "Generator.hpp"

#include <glm/glm.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include <array>
#include <fstream>
#include <glm/vec3.hpp>
#include <iostream>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using std::vector;

namespace generator {

struct Vec3Hash {
  std::size_t operator()(const vec3& v) const {
    std::size_t hx = std::hash<float>{}(v.x);
    std::size_t hy = std::hash<float>{}(v.y);
    std::size_t hz = std::hash<float>{}(v.z);
    return ((hx ^ (hy << 1)) >> 1) ^ (hz << 1);
  }
};

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
  Model model;
  AttributeIndexer<glm::vec3> posIdx;
  AttributeIndexer<glm::vec3> normIdx;
  AttributeIndexer<glm::vec2> uvIdx;

  float sliceSize = 2 * M_PI / slices;
  float stackSize = height / stacks;

  glm::vec3 tip = {0, height, 0};
  glm::vec3 baseMiddle = {0, 0, 0};

  for (int stack = 0; stack < stacks; stack++) {
    float currentRadius = radius - stack * radius / stacks;
    float nextRadius = radius - (stack + 1) * radius / stacks;

    for (int slice = 0; slice < slices; slice++) {
      float u1 = (float)slice / slices;
      float u2 = (float)(slice + 1) / slices;
      float v1 = (float)stack / stacks;
      float v2 = (float)(stack + 1) / stacks;

      glm::vec3 bottomLeft =
          polarToCartesian(currentRadius, slice * sliceSize, stack * stackSize);
      glm::vec3 bottomRight = polarToCartesian(
          currentRadius, (slice + 1) * sliceSize, stack * stackSize);
      glm::vec3 topLeft = polarToCartesian(nextRadius, slice * sliceSize,
                                           (stack + 1) * stackSize);
      glm::vec3 topRight = polarToCartesian(nextRadius, (slice + 1) * sliceSize,
                                            (stack + 1) * stackSize);

      glm::vec3 sideNormal = glm::normalize(
          glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft));

      glm::vec2 uvBottomLeft = {u1, v1};
      glm::vec2 uvBottomRight = {u2, v1};
      glm::vec2 uvTopLeft = {u1, v2};
      glm::vec2 uvTopRight = {u2, v2};

      unsigned int i0 = posIdx.add(bottomLeft);
      unsigned int i1 = posIdx.add(bottomRight);
      unsigned int i2 = posIdx.add(topLeft);
      unsigned int i3 = posIdx.add(topRight);

      unsigned int t0 = uvIdx.add(uvBottomLeft);
      unsigned int t1 = uvIdx.add(uvBottomRight);
      unsigned int t2 = uvIdx.add(uvTopLeft);
      unsigned int t3 = uvIdx.add(uvTopRight);

      unsigned int n = normIdx.add(sideNormal);

      model.indices.push_back({i0, t0, n});
      model.indices.push_back({i1, t1, n});
      model.indices.push_back({i2, t2, n});

      model.indices.push_back({i2, t2, n});
      model.indices.push_back({i1, t1, n});
      model.indices.push_back({i3, t3, n});
    }
  }

  for (int slice = 0; slice < slices; slice++) {
    float u1 = (float)slice / slices;
    float u2 = (float)(slice + 1) / slices;

    glm::vec3 baseBottomLeft = polarToCartesian(radius, slice * sliceSize, 0);
    glm::vec3 baseBottomRight =
        polarToCartesian(radius, (slice + 1) * sliceSize, 0);

    glm::vec2 uvBottomLeft = {u1, 0};
    glm::vec2 uvBottomRight = {u2, 0};
    glm::vec2 uvCenter = {0.5f, 0.5f};

    unsigned int i0 = posIdx.add(baseMiddle);
    unsigned int i1 = posIdx.add(baseBottomRight);
    unsigned int i2 = posIdx.add(baseBottomLeft);

    unsigned int t0 = uvIdx.add(uvCenter);
    unsigned int t1 = uvIdx.add(uvBottomRight);
    unsigned int t2 = uvIdx.add(uvBottomLeft);

    glm::vec3 baseNormal = {0, -1, 0};

    model.indices.push_back({i0, t0, normIdx.add(baseNormal)});
    model.indices.push_back({i1, t1, normIdx.add(baseNormal)});
    model.indices.push_back({i2, t2, normIdx.add(baseNormal)});
  }

  model.positions = std::move(posIdx.data);
  model.normals = std::move(normIdx.data);
  model.texcoords = std::move(uvIdx.data);

  return model;
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
  Model model;
  AttributeIndexer<glm::vec3> posIdx;
  AttributeIndexer<glm::vec3> normIdx;
  AttributeIndexer<glm::vec2> uvIdx;

  float sliceSize = 2 * M_PI / slices;
  float stackSize = M_PI / stacks;

  for (int slice = 0; slice < slices; slice++) {
    for (int stack = 0; stack < stacks; stack++) {
      glm::vec3 bottomLeft = sphericalToCartesian(radius, slice * sliceSize,
                                                  stack * stackSize - M_PI_2);
      glm::vec3 bottomRight = sphericalToCartesian(
          radius, (slice + 1) * sliceSize, stack * stackSize - M_PI_2);
      glm::vec3 topLeft = sphericalToCartesian(
          radius, slice * sliceSize, (stack + 1) * stackSize - M_PI_2);
      glm::vec3 topRight = sphericalToCartesian(
          radius, (slice + 1) * sliceSize, (stack + 1) * stackSize - M_PI_2);

      glm::vec3 normBottomLeft = glm::normalize(bottomLeft);
      glm::vec3 normBottomRight = glm::normalize(bottomRight);
      glm::vec3 normTopLeft = glm::normalize(topLeft);
      glm::vec3 normTopRight = glm::normalize(topRight);

      glm::vec2 uvBottomLeft = {(float)slice / slices, (float)(stack) / stacks};
      glm::vec2 uvBottomRight = {(float)(slice + 1) / slices,
                                 (float)(stack) / stacks};
      glm::vec2 uvTopLeft = {(float)slice / slices,
                             (float)(stack + 1) / stacks};
      glm::vec2 uvTopRight = {(float)(slice + 1) / slices,
                              (float)(stack + 1) / stacks};

      unsigned int i0 = posIdx.add(bottomLeft);
      unsigned int i1 = posIdx.add(bottomRight);
      unsigned int i2 = posIdx.add(topLeft);
      unsigned int i3 = posIdx.add(topRight);

      unsigned int n0 = normIdx.add(normBottomLeft);
      unsigned int n1 = normIdx.add(normBottomRight);
      unsigned int n2 = normIdx.add(normTopLeft);
      unsigned int n3 = normIdx.add(normTopRight);

      unsigned int t0 = uvIdx.add(uvBottomLeft);
      unsigned int t1 = uvIdx.add(uvBottomRight);
      unsigned int t2 = uvIdx.add(uvTopLeft);
      unsigned int t3 = uvIdx.add(uvTopRight);

      model.indices.push_back({i0, t0, n0});
      model.indices.push_back({i1, t1, n1});
      model.indices.push_back({i2, t2, n2});

      model.indices.push_back({i2, t2, n2});
      model.indices.push_back({i1, t1, n1});
      model.indices.push_back({i3, t3, n3});
    }
  }

  model.positions = std::move(posIdx.data);
  model.normals = std::move(normIdx.data);
  model.texcoords = std::move(uvIdx.data);

  return model;
}

/**
 * Generates a 3D model of a plane.
 *
 * @param length The length of the plane.
 * @param divisions The number of divisions along each axis.
 * @return A Model object containing the vertices of the generated plane.
 */
Model Plane(float length, int divisions) {
  Model model;

  AttributeIndexer<glm::vec3> posIdx;
  AttributeIndexer<glm::vec3> normIdx;
  AttributeIndexer<glm::vec2> uvIdx;

  float half = length / 2.0f;
  float step = length / divisions;

  glm::vec3 normal(0, 1, 0);

  for (int x = 0; x < divisions; x++) {
    for (int z = 0; z < divisions; z++) {
      float x0 = -half + x * step;
      float z0 = -half + z * step;
      float x1 = x0 + step;
      float z1 = z0 + step;

      glm::vec3 p0(x0, 0, z0);
      glm::vec3 p1(x1, 0, z0);
      glm::vec3 p2(x0, 0, z1);
      glm::vec3 p3(x1, 0, z1);

      float xTexCoord0 = (x0 + half) / length;
      float xTexCoord1 = (x1 + half) / length;
      float zTexCoord0 = (z0 + half) / length;
      float zTexCoord1 = (z1 + half) / length;

      glm::vec2 uv0(xTexCoord0, zTexCoord0);
      glm::vec2 uv1(xTexCoord1, zTexCoord0);
      glm::vec2 uv2(xTexCoord0, zTexCoord1);
      glm::vec2 uv3(xTexCoord1, zTexCoord1);

      unsigned int i0 = posIdx.add(p0);
      unsigned int i1 = posIdx.add(p1);
      unsigned int i2 = posIdx.add(p2);
      unsigned int i3 = posIdx.add(p3);

      unsigned int n = normIdx.add(normal);

      unsigned int t0 = uvIdx.add(uv0);
      unsigned int t1 = uvIdx.add(uv1);
      unsigned int t2 = uvIdx.add(uv2);
      unsigned int t3 = uvIdx.add(uv3);

      model.indices.push_back({i1, t1, n});
      model.indices.push_back({i0, t0, n});
      model.indices.push_back({i2, t2, n});

      model.indices.push_back({i1, t1, n});
      model.indices.push_back({i2, t2, n});
      model.indices.push_back({i3, t3, n});
    }
  }

  model.positions = std::move(posIdx.data);
  model.normals = std::move(normIdx.data);
  model.texcoords = std::move(uvIdx.data);
  return model;
}

/**
 * @brief Generates a 3D box model.
 *
 * @param size The size of the box along each axis.
 * @param divisions The number of divisions along each axis.
 * @return Model The generated 3D box model containing the vertices.
 */
Model Box(float size, int divisions) {
  Model model;
  AttributeIndexer<glm::vec3> posIdx;
  AttributeIndexer<glm::vec3> normIdx;
  AttributeIndexer<glm::vec2> uvIdx;

  float halfSize = size / 2.0f;
  float step = size / divisions;

  const glm::vec3 normals[6] = {
      {0, 0, 1},   // Front
      {0, 0, -1},  // Back
      {-1, 0, 0},  // Left
      {1, 0, 0},   // Right
      {0, 1, 0},   // Top
      {0, -1, 0}   // Bottom
  };

  // For each face of the cube
  for (int face = 0; face < 6; face++) {
    unsigned int n = normIdx.add(normals[face]);

    for (int i = 0; i < divisions; i++) {
      for (int j = 0; j < divisions; j++) {
        float u1 = -halfSize + i * step;
        float v1 = -halfSize + j * step;
        float u2 = u1 + step;
        float v2 = v1 + step;

        float tu1 = (float)i / divisions;
        float tv1 = (float)j / divisions;
        float tu2 = (float)(i + 1) / divisions;
        float tv2 = (float)(j + 1) / divisions;

        glm::vec3 p[4];
        glm::vec2 uv[4];

        uv[0] = {tu1, tv1};
        uv[1] = {tu2, tv1};
        uv[2] = {tu1, tv2};
        uv[3] = {tu2, tv2};

        switch (face) {
          case 0:  // Front face
            p[0] = {u1, v1, halfSize};
            p[1] = {u2, v1, halfSize};
            p[2] = {u1, v2, halfSize};
            p[3] = {u2, v2, halfSize};
            break;
          case 1:  // Back face
            p[0] = {u2, v1, -halfSize};
            p[1] = {u1, v1, -halfSize};
            p[2] = {u2, v2, -halfSize};
            p[3] = {u1, v2, -halfSize};

            uv[0] = {tu2, tv1};
            uv[1] = {tu1, tv1};
            uv[2] = {tu2, tv2};
            uv[3] = {tu1, tv2};
            break;
          case 2:  // Left face
            p[0] = {-halfSize, v1, u1};
            p[1] = {-halfSize, v1, u2};
            p[2] = {-halfSize, v2, u1};
            p[3] = {-halfSize, v2, u2};

            uv[0] = {1.0f - tu1, tv1};
            uv[1] = {1.0f - tu2, tv1};
            uv[2] = {1.0f - tu1, tv2};
            uv[3] = {1.0f - tu2, tv2};
            break;
          case 3:  // Right face
            p[0] = {halfSize, v1, u2};
            p[1] = {halfSize, v1, u1};
            p[2] = {halfSize, v2, u2};
            p[3] = {halfSize, v2, u1};

            uv[0] = {tu2, tv1};
            uv[1] = {tu1, tv1};
            uv[2] = {tu2, tv2};
            uv[3] = {tu1, tv2};
            break;
          case 4:  // Top face
            p[0] = {u1, halfSize, v2};
            p[1] = {u2, halfSize, v2};
            p[2] = {u1, halfSize, v1};
            p[3] = {u2, halfSize, v1};
            break;
          case 5:  // Bottom face
            p[0] = {u1, -halfSize, v1};
            p[1] = {u2, -halfSize, v1};
            p[2] = {u1, -halfSize, v2};
            p[3] = {u2, -halfSize, v2};

            uv[0] = {tu1, tv2};
            uv[1] = {tu2, tv2};
            uv[2] = {tu1, tv1};
            uv[3] = {tu2, tv1};
            break;
        }

        unsigned int i0 = posIdx.add(p[0]);
        unsigned int i1 = posIdx.add(p[1]);
        unsigned int i2 = posIdx.add(p[2]);
        unsigned int i3 = posIdx.add(p[3]);

        unsigned int t0 = uvIdx.add(uv[0]);
        unsigned int t1 = uvIdx.add(uv[1]);
        unsigned int t2 = uvIdx.add(uv[2]);
        unsigned int t3 = uvIdx.add(uv[3]);

        model.indices.push_back({i0, t0, n});
        model.indices.push_back({i1, t1, n});
        model.indices.push_back({i2, t2, n});

        model.indices.push_back({i2, t2, n});
        model.indices.push_back({i1, t1, n});
        model.indices.push_back({i3, t3, n});
      }
    }
  }

  model.positions = std::move(posIdx.data);
  model.normals = std::move(normIdx.data);
  model.texcoords = std::move(uvIdx.data);
  return model;
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
  Model model;
  AttributeIndexer<glm::vec3> posIdx;
  AttributeIndexer<glm::vec3> normIdx;
  AttributeIndexer<glm::vec2> uvIdx;

  float sliceSize = 2 * M_PI / slices;
  float stackSize = height / stacks;
  float halfHeight = height / 2.0f;

  // Sides
  for (int slice = 0; slice < slices; slice++) {
    float angle1 = slice * sliceSize;
    float angle2 = (slice + 1) * sliceSize;

    glm::vec3 normal1 = glm::normalize(glm::vec3(cos(angle1), 0, sin(angle1)));
    glm::vec3 normal2 = glm::normalize(glm::vec3(cos(angle2), 0, sin(angle2)));

    float u0 = (float)slice / slices;
    float u1 = (float)(slice + 1) / slices;

    for (int stack = 0; stack < stacks; stack++) {
      float y0 = stack * stackSize - halfHeight;
      float y1 = (stack + 1) * stackSize - halfHeight;
      float v0 = (float)stack / stacks;
      float v1 = (float)(stack + 1) / stacks;

      glm::vec3 bl = glm::vec3(radius * cos(angle1), y0, radius * sin(angle1));
      glm::vec3 br = glm::vec3(radius * cos(angle2), y0, radius * sin(angle2));
      glm::vec3 tl = glm::vec3(radius * cos(angle1), y1, radius * sin(angle1));
      glm::vec3 tr = glm::vec3(radius * cos(angle2), y1, radius * sin(angle2));

      unsigned int i0 = posIdx.add(bl);
      unsigned int i1 = posIdx.add(br);
      unsigned int i2 = posIdx.add(tl);
      unsigned int i3 = posIdx.add(tr);

      unsigned int n0 = normIdx.add(normal1);
      unsigned int n1 = normIdx.add(normal2);
      unsigned int n2 = normIdx.add(normal1);
      unsigned int n3 = normIdx.add(normal2);

      unsigned int t0 = uvIdx.add({u0, v0});
      unsigned int t1 = uvIdx.add({u1, v0});
      unsigned int t2 = uvIdx.add({u0, v1});
      unsigned int t3 = uvIdx.add({u1, v1});

      model.indices.push_back({i0, t0, n0});
      model.indices.push_back({i2, t2, n2});
      model.indices.push_back({i1, t1, n1});

      model.indices.push_back({i2, t2, n2});
      model.indices.push_back({i3, t3, n3});
      model.indices.push_back({i1, t1, n1});
    }
  }

  // Base and top
  glm::vec3 baseCenter = glm::vec3(0, -halfHeight, 0);
  glm::vec3 topCenter = glm::vec3(0, halfHeight, 0);
  glm::vec3 baseNormal = glm::vec3(0, -1, 0);
  glm::vec3 topNormal = glm::vec3(0, 1, 0);
  unsigned int baseCenterIdx = posIdx.add(baseCenter);
  unsigned int topCenterIdx = posIdx.add(topCenter);
  unsigned int baseNormIdx = normIdx.add(baseNormal);
  unsigned int topNormIdx = normIdx.add(topNormal);
  unsigned int baseUvIdx = uvIdx.add({0.5f, 0.5f});
  unsigned int topUvIdx = uvIdx.add({0.5f, 0.5f});

  for (int slice = 0; slice < slices; slice++) {
    float angle1 = slice * sliceSize;
    float angle2 = (slice + 1) * sliceSize;

    glm::vec3 b1 =
        glm::vec3(radius * cos(angle1), -halfHeight, radius * sin(angle1));
    glm::vec3 b2 =
        glm::vec3(radius * cos(angle2), -halfHeight, radius * sin(angle2));
    glm::vec3 t1 =
        glm::vec3(radius * cos(angle1), halfHeight, radius * sin(angle1));
    glm::vec3 t2 =
        glm::vec3(radius * cos(angle2), halfHeight, radius * sin(angle2));

    glm::vec2 uv_b1 = {0.5f + 0.5f * cos(angle1), 0.5f + 0.5f * sin(angle1)};
    glm::vec2 uv_b2 = {0.5f + 0.5f * cos(angle2), 0.5f + 0.5f * sin(angle2)};
    glm::vec2 uv_t1 = uv_b1;
    glm::vec2 uv_t2 = uv_b2;

    unsigned int i_b1 = posIdx.add(b1);
    unsigned int i_b2 = posIdx.add(b2);
    unsigned int i_t1 = posIdx.add(t1);
    unsigned int i_t2 = posIdx.add(t2);

    unsigned int t_b1 = uvIdx.add(uv_b1);
    unsigned int t_b2 = uvIdx.add(uv_b2);
    unsigned int t_t1 = uvIdx.add(uv_t1);
    unsigned int t_t2 = uvIdx.add(uv_t2);

    // Base triangle
    model.indices.push_back({baseCenterIdx, baseUvIdx, baseNormIdx});
    model.indices.push_back({i_b1, t_b1, baseNormIdx});
    model.indices.push_back({i_b2, t_b2, baseNormIdx});

    // Top triangle
    model.indices.push_back({topCenterIdx, topUvIdx, topNormIdx});
    model.indices.push_back({i_t2, t_t2, topNormIdx});
    model.indices.push_back({i_t1, t_t1, topNormIdx});
  }

  model.positions = std::move(posIdx.data);
  model.normals = std::move(normIdx.data);
  model.texcoords = std::move(uvIdx.data);

  return model;
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
  Model model;
  AttributeIndexer<vec3> posIdx;
  AttributeIndexer<vec3> normIdx;
  AttributeIndexer<vec2> uvIdx;

  for (int stack = 0; stack < stacks; stack++) {
    float theta1 = 2.0f * M_PI * stack / stacks;
    float theta2 = 2.0f * M_PI * (stack + 1) / stacks;

    for (int slice = 0; slice < slices; slice++) {
      float phi1 = 2.0f * M_PI * slice / slices;
      float phi2 = 2.0f * M_PI * (slice + 1) / slices;

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

      // Normals
      vec3 center1 = vec3(radius * cos(theta1), 0, radius * sin(theta1));
      vec3 center2 = vec3(radius * cos(theta2), 0, radius * sin(theta2));

      vec3 n_tl = normalize(topLeft - center1);
      vec3 n_tr = normalize(topRight - center2);
      vec3 n_bl = normalize(bottomLeft - center1);
      vec3 n_br = normalize(bottomRight - center2);

      // UVs
      vec2 uv_tl = vec2((float)stack / stacks, (float)slice / slices);
      vec2 uv_tr = vec2((float)(stack + 1) / stacks, (float)slice / slices);
      vec2 uv_bl = vec2((float)stack / stacks, (float)(slice + 1) / slices);
      vec2 uv_br =
          vec2((float)(stack + 1) / stacks, (float)(slice + 1) / slices);

      unsigned int i0 = posIdx.add(topLeft);
      unsigned int i1 = posIdx.add(bottomLeft);
      unsigned int i2 = posIdx.add(bottomRight);
      unsigned int i3 = posIdx.add(topRight);

      unsigned int n0 = normIdx.add(n_tl);
      unsigned int n1 = normIdx.add(n_bl);
      unsigned int n2 = normIdx.add(n_br);
      unsigned int n3 = normIdx.add(n_tr);

      unsigned int t0 = uvIdx.add(uv_tl);
      unsigned int t1 = uvIdx.add(uv_bl);
      unsigned int t2 = uvIdx.add(uv_br);
      unsigned int t3 = uvIdx.add(uv_tr);

      model.indices.push_back({i0, t0, n0});
      model.indices.push_back({i1, t1, n1});
      model.indices.push_back({i2, t2, n2});

      model.indices.push_back({i0, t0, n0});
      model.indices.push_back({i2, t2, n2});
      model.indices.push_back({i3, t3, n3});
    }
  }

  model.positions = std::move(posIdx.data);
  model.normals = std::move(normIdx.data);
  model.texcoords = std::move(uvIdx.data);

  return model;
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

  VertexIndexer<vec3, Vec3Hash> indexer;

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

      indexer.indices.push_back(indexer.addVertex(a * radius));
      indexer.indices.push_back(indexer.addVertex(b * radius));
      indexer.indices.push_back(indexer.addVertex(c * radius));
    }

    icosahedronVertices = newVertices;
  }

  for (const auto& vertex : icosahedronVertices) {
    indexer.indices.push_back(indexer.addVertex(vertex * radius));
  }

  return {};
}

/**
 * @brief Generates a Bezier patch from the given control points.
 *
 * @param control_points The control points of the Bezier patch.
 * @param tesselation_level The level of tessellation for the patch.
 * @return A vector of vertices representing the Bezier patch.
 */
std::vector<vec3> BezierPatch(const std::array<vec3, 16>& control_points,
                              const size_t tesselation_level) {
  std::vector<vec3> vertices((tesselation_level + 1) * (tesselation_level + 1));

  glm::mat4 bernstein_matrix =
      glm::mat4(-1, 3, -3, 1, 3, -6, 3, 0, -3, 3, 0, 0, 1, 0, 0, 0);

  for (int i = 0; i < 3; i++) {
    glm::mat4 points_matrix = glm::mat4(
        control_points[0][i], control_points[1][i], control_points[2][i],
        control_points[3][i], control_points[4][i], control_points[5][i],
        control_points[6][i], control_points[7][i], control_points[8][i],
        control_points[9][i], control_points[10][i], control_points[11][i],
        control_points[12][i], control_points[13][i], control_points[14][i],
        control_points[15][i]);

    glm::mat4 result_matrix =
        bernstein_matrix * points_matrix * bernstein_matrix;

    for (int j = 0; j <= tesselation_level; j++) {
      for (int k = 0; k <= tesselation_level; k++) {
        float u = static_cast<float>(j) / tesselation_level;
        float v = static_cast<float>(k) / tesselation_level;

        vec4 u_vector = {u * u * u, u * u, u, 1};
        vec4 v_vector = {v * v * v, v * v, v, 1};

        vertices[j * (tesselation_level + 1) + k][i] =
            glm::dot(v_vector, result_matrix * u_vector);
      }
    }
  }

  return vertices;
}

/**
 * @brief Generates a 3D model of a Bezier surface from a patch file.
 *
 * @param patch The path to the patch file.
 * @param tessellation The number of subdivisions for tessellation.
 * @return A Model object containing the vertices and indices of the generated
 * surface.
 */
Model BezierSurface(const std::string patch, int tessellation) {
  // Read and parse the patch file

  std::ifstream file(patch);

  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << patch << std::endl;
    return {};
  }

  int32_t patch_count;
  file >> patch_count;
  std::vector<std::vector<unsigned int>> patches(patch_count,
                                                 std::vector<unsigned int>(16));
  for (int i = 0; i < patch_count; i++) {
    for (int j = 0; j < 16; ++j) {
      size_t idx;
      file >> idx;
      file.ignore();
      patches[i][j] = idx;
    }
  }

  int32_t control_points_count;
  file >> control_points_count;
  std::vector<vec3> control_points;
  control_points.reserve(control_points_count);

  for (int i = 0; i < control_points_count; i++) {
    float x, y, z;
    file >> x;
    file.ignore();
    file >> y;
    file.ignore();
    file >> z;
    file.ignore();

    control_points.push_back(vec3(x, y, z));
  }

  file.close();

  // Generate model
  VertexIndexer<vec3, Vec3Hash> indexer;

  uint32_t start = 0;
  for (const auto& patch : patches) {
    std::array<glm::vec3, 16> patch_vertices;
    for (int i = 0; i < patch.size(); ++i) {
      patch_vertices[i] = control_points[patch[i]];
    }

    std::vector<vec3> vertices = BezierPatch(patch_vertices, tessellation);
    std::vector<std::vector<uint32_t>> index_grid(
        tessellation + 1, std::vector<uint32_t>(tessellation + 1));

    for (int j = 0; j <= tessellation; ++j) {
      for (int k = 0; k <= tessellation; ++k) {
        int idx = j * (tessellation + 1) + k;
        index_grid[j][k] = indexer.addVertex(vertices[idx]);
      }
    }

    for (int z = 0; z < tessellation; ++z) {
      for (int x = 0; x < tessellation; ++x) {
        uint32_t top_left = index_grid[z][x];
        uint32_t top_right = index_grid[z][x + 1];
        uint32_t bottom_left = index_grid[z + 1][x];
        uint32_t bottom_right = index_grid[z + 1][x + 1];

        indexer.indices.push_back(top_left);
        indexer.indices.push_back(bottom_left);
        indexer.indices.push_back(bottom_right);

        indexer.indices.push_back(top_left);
        indexer.indices.push_back(bottom_right);
        indexer.indices.push_back(top_right);
      }
    }
  }

  return {};
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
  if (!file.is_open()) return false;

  for (const auto& v : model.positions)
    file << "v " << v.x << " " << v.y << " " << v.z << "\n";

  for (const auto& vt : model.texcoords)
    file << "vt " << vt.x << " " << vt.y << "\n";

  for (const auto& vn : model.normals)
    file << "vn " << vn.x << " " << vn.y << " " << vn.z << "\n";

  for (size_t i = 0; i < model.indices.size(); i += 3) {
    auto idx = [&](const Model::IndexTriplet& t) {
      return std::to_string(t.posIndex + 1) + "/" +
             std::to_string(t.uvIndex + 1) + "/" +
             std::to_string(t.normIndex + 1);
    };
    file << "f " << idx(model.indices[i]) << " " << idx(model.indices[i + 1])
         << " " << idx(model.indices[i + 2]) << "\n";
  }

  return true;
}

}  // namespace generator
