#include "Model.hpp"

#include <fstream>
#include <optional>

#include "debug/Logger.hpp"

using std::optional;

static debug::Logger logger;

optional<Model> Model::loadModel(const string& filename) {
  Model model;

  // Load model from file
  std::ifstream file(filename);
  if (!file.is_open()) {
    logger.error("Failed to open file: " + filename);
    return {};
  }

  string line;
  while (std::getline(file, line)) {
    if (line[0] == 'v') {
      vec3 vertex;
      sscanf(line.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
      model.addVertex(vertex);
    }
  }

  return model;
}

void Model::addVertex(vec3 vertex) { vertices.push_back(vertex); }