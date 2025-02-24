#include "Model.hpp"

#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

#include "debug/Logger.hpp"

using std::optional;

static debug::Logger logger;

optional<Model> loadModel(const string& filename) {
  Model model;

  // Load model from file
  std::ifstream file(filename);
  if (!file.is_open()) {
    logger.error("Failed to open file: " + filename);
    return {};
  }

  string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string type;
    iss >> type;

    if (type == "v") {
      vec3 vertex;
      iss >> vertex.x >> vertex.y >> vertex.z;
      model.addVertex(vertex);
    }
  }

  model.setName(filename);

  file.close();

  return model;
}

void Model::addVertex(vec3 vertex) { vertices.push_back(vertex); }

void Model::render() {
  glBegin(GL_TRIANGLES);

  for (const vec3& vertex : vertices) {
    glVertex3f(vertex.x, vertex.y, vertex.z);
  }

  glEnd();
}