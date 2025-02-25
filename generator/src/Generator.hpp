#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Model {
  std::vector<glm::vec3> vertices;
};

namespace generator {
Model Cone(float radius, float height, int slices, int stacks);
Model Sphere(float radius, int slices, int stacks);
Model Plane(float length, int divisions);
Model Box(float size, int divisions);
Model Cylinder(float radius, float height, int slices, int stacks);
bool Export(const Model& model, const std::string& filename);
}  // namespace generator