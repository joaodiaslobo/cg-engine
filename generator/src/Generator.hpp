#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

struct Model {
  std::vector<glm::vec3> vertices;
  std::vector<unsigned int> indices;
};

namespace std {
template <>
struct hash<glm::vec3> {
  std::size_t operator()(const glm::vec3& v) const {
    std::size_t h1 = std::hash<float>{}(v.x);
    std::size_t h2 = std::hash<float>{}(v.y);
    std::size_t h3 = std::hash<float>{}(v.z);
    return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
  }
};
}  // namespace std

namespace generator {
Model Cone(float radius, float height, int slices, int stacks);
Model Sphere(float radius, int slices, int stacks);
Model Plane(float length, int divisions);
Model Box(float size, int divisions);
Model Cylinder(float radius, float height, int slices, int stacks);
Model Torus(float radius, float tubeRadius, int slices, int stacks);
Model Icosphere(float radius, int subdivisions);
Model BezierSurface(std::string patch, int tessellation);
bool Export(const Model& model, const std::string& filename);

template <typename Vertex, typename Hash = std::hash<Vertex>>
class VertexIndexer {
 public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  unsigned int addVertex(const Vertex& v) {
    auto it = vertexToIndex.find(v);
    if (it != vertexToIndex.end()) {
      return it->second;
    } else {
      unsigned int index = static_cast<unsigned int>(vertices.size());
      vertices.push_back(v);
      vertexToIndex[v] = index;
      return index;
    }
  }

 private:
  std::unordered_map<Vertex, unsigned int, Hash> vertexToIndex;
};
}  // namespace generator