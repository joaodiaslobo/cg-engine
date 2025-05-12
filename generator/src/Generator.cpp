#include "Generator.hpp"

#include <glm/glm.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include <array>
#include <fstream>
#include <glm/vec3.hpp>
#include <iostream>

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
  VertexIndexer<vec3, Vec3Hash> indexer;

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

      indexer.indices.push_back(indexer.addVertex(topLeft));
      indexer.indices.push_back(indexer.addVertex(bottomLeft));
      indexer.indices.push_back(indexer.addVertex(bottomRight));

      indexer.indices.push_back(indexer.addVertex(topLeft));
      indexer.indices.push_back(indexer.addVertex(bottomRight));
      indexer.indices.push_back(indexer.addVertex(topRight));
    }

    vec3 baseBottomLeft = polarToCartesian(radius, slice * sliceSize, 0);
    vec3 baseBottomRight = polarToCartesian(radius, (slice + 1) * sliceSize, 0);

    indexer.indices.push_back(indexer.addVertex(baseMiddle));
    indexer.indices.push_back(indexer.addVertex(baseBottomRight));
    indexer.indices.push_back(indexer.addVertex(baseBottomLeft));
  }

  return {};
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
  VertexIndexer<vec3, Vec3Hash> indexer;

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

      indexer.indices.push_back(indexer.addVertex(topLeft));
      indexer.indices.push_back(indexer.addVertex(bottomLeft));
      indexer.indices.push_back(indexer.addVertex(bottomRight));

      indexer.indices.push_back(indexer.addVertex(topLeft));
      indexer.indices.push_back(indexer.addVertex(bottomRight));
      indexer.indices.push_back(indexer.addVertex(topRight));
    }
  }
  return {};
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
  float halfSize = size / 2.0f;
  float step = size / divisions;
  VertexIndexer<vec3, Vec3Hash> indexer;

  // VERTICES

  for (int i = 0; i < divisions; ++i) {
    for (int j = 0; j < divisions; ++j) {
      float v1 = -halfSize + i * step;
      float u1 = -halfSize + j * step;
      float v2 = v1 + step;
      float u2 = u1 + step;

      indexer.indices.push_back(indexer.addVertex(vec3(v1, u1, halfSize)));
      indexer.indices.push_back(indexer.addVertex(vec3(v2, u1, halfSize)));
      indexer.indices.push_back(indexer.addVertex(vec3(v1, u2, halfSize)));

      indexer.indices.push_back(indexer.addVertex(vec3(v1, u2, halfSize)));
      indexer.indices.push_back(indexer.addVertex(vec3(v2, u1, halfSize)));
      indexer.indices.push_back(indexer.addVertex(vec3(v2, u2, halfSize)));

      indexer.indices.push_back(indexer.addVertex(vec3(v1, u1, -halfSize)));
      indexer.indices.push_back(indexer.addVertex(vec3(v1, u2, -halfSize)));
      indexer.indices.push_back(indexer.addVertex(vec3(v2, u1, -halfSize)));

      indexer.indices.push_back(indexer.addVertex(vec3(v1, u2, -halfSize)));
      indexer.indices.push_back(indexer.addVertex(vec3(v2, u1, -halfSize)));
      indexer.indices.push_back(indexer.addVertex(vec3(v2, u1, -halfSize)));

      indexer.indices.push_back(indexer.addVertex(vec3(-halfSize, v1, u1)));
      indexer.indices.push_back(indexer.addVertex(vec3(-halfSize, v1, u2)));
      indexer.indices.push_back(indexer.addVertex(vec3(-halfSize, v2, u1)));

      indexer.indices.push_back(indexer.addVertex(vec3(-halfSize, v1, u2)));
      indexer.indices.push_back(indexer.addVertex(vec3(-halfSize, v2, u2)));
      indexer.indices.push_back(indexer.addVertex(vec3(-halfSize, v2, u1)));

      indexer.indices.push_back(indexer.addVertex(vec3(halfSize, v1, u1)));
      indexer.indices.push_back(indexer.addVertex(vec3(halfSize, v2, u1)));
      indexer.indices.push_back(indexer.addVertex(vec3(halfSize, v1, u2)));

      indexer.indices.push_back(indexer.addVertex(vec3(halfSize, v1, u2)));
      indexer.indices.push_back(indexer.addVertex(vec3(halfSize, v2, u1)));
      indexer.indices.push_back(indexer.addVertex(vec3(halfSize, v2, u2)));

      indexer.indices.push_back(indexer.addVertex(vec3(v1, halfSize, u1)));
      indexer.indices.push_back(indexer.addVertex(vec3(v1, halfSize, u2)));
      indexer.indices.push_back(indexer.addVertex(vec3(v2, halfSize, u1)));

      indexer.indices.push_back(indexer.addVertex(vec3(v1, halfSize, u2)));
      indexer.indices.push_back(indexer.addVertex(vec3(v2, halfSize, u2)));
      indexer.indices.push_back(indexer.addVertex(vec3(v2, halfSize, u1)));

      indexer.indices.push_back(indexer.addVertex(vec3(v1, -halfSize, u1)));
      indexer.indices.push_back(indexer.addVertex(vec3(v2, -halfSize, u1)));
      indexer.indices.push_back(indexer.addVertex(vec3(v1, -halfSize, u2)));

      indexer.indices.push_back(indexer.addVertex(vec3(v1, -halfSize, u2)));
      indexer.indices.push_back(indexer.addVertex(vec3(v2, -halfSize, u1)));
      indexer.indices.push_back(indexer.addVertex(vec3(v2, -halfSize, u2)));
    }
  }

  return Model{};
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
  VertexIndexer<vec3, Vec3Hash> indexer;

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

      indexer.indices.push_back(indexer.addVertex(bottomLeft));
      indexer.indices.push_back(indexer.addVertex(bottomRight));
      indexer.indices.push_back(indexer.addVertex(topLeft));

      indexer.indices.push_back(indexer.addVertex(topLeft));
      indexer.indices.push_back(indexer.addVertex(bottomRight));
      indexer.indices.push_back(indexer.addVertex(topRight));
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

    indexer.indices.push_back(indexer.addVertex(baseMiddle));
    indexer.indices.push_back(indexer.addVertex(baseRight));
    indexer.indices.push_back(indexer.addVertex(baseLeft));

    indexer.indices.push_back(indexer.addVertex(topMiddle));
    indexer.indices.push_back(indexer.addVertex(topLeft));
    indexer.indices.push_back(indexer.addVertex(topRight));
  }

  return {};
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
  VertexIndexer<vec3, Vec3Hash> indexer;

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

      indexer.indices.push_back(indexer.addVertex(topLeft));
      indexer.indices.push_back(indexer.addVertex(bottomLeft));
      indexer.indices.push_back(indexer.addVertex(bottomRight));

      indexer.indices.push_back(indexer.addVertex(topLeft));
      indexer.indices.push_back(indexer.addVertex(bottomRight));
      indexer.indices.push_back(indexer.addVertex(topRight));
    }
  }
  return {};
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
