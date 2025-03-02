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
  if (filename.size() >= 3 &&
      filename.compare(filename.size() - 3, 3, ".3d") == 0) {
    return load3dModel(filename);
  } else if (filename.size() >= 4 &&
             filename.compare(filename.size() - 4, 4, ".obj") == 0) {
    return loadObjModel(filename);
  }

  logger.error("Failed to load model: " + filename);
  return {};
}

size_t parseFirstIndex(std::string_view string) {
  if (const size_t pos = string.find('/'); pos != std::string::npos) {
    return std::stoi(std::string(string.substr(0, pos)));
  }
  return std::stoi(std::string(string));
}

optional<Model> load3dModel(const string& filename) {
  Model model;

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

optional<Model> loadObjModel(const string& filename) {
  Model model;

  std::ifstream file(filename);
  if (!file.is_open()) {
    logger.error("Failed to open file: " + filename);
    return {};
  }

  string line;
  vector<vec3> global_vertices;

  while (std::getline(file, line)) {
    if (line[0] == '#') {
      continue;
    }

    std::istringstream stream(line);
    std::string type;
    stream >> type;

    if (type == "v") {
      vec3 vertex;
      stream >> vertex.x >> vertex.y >> vertex.z;
      global_vertices.push_back(vertex);
    } else if (type == "f") {
      std::string vertex1, vertex2, vertex3;
      stream >> vertex1 >> vertex2 >> vertex3;

      size_t i1 = parseFirstIndex(vertex1) - 1;
      size_t i2 = parseFirstIndex(vertex2) - 1;
      size_t i3 = parseFirstIndex(vertex3) - 1;

      model.addVertex(global_vertices[i1]);
      model.addVertex(global_vertices[i2]);
      model.addVertex(global_vertices[i3]);
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