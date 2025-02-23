#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <string>

struct Model {
    std::vector<glm::vec3> vertices;
};

namespace generator {
    Model Cone(float radius, float height, int slices, int stacks);
    bool Export(const Model& model, const std::string& filename);
}