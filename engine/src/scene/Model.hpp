#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <vector>

using glm::vec3;
using std::optional;
using std::string;
using std::vector;

class Model {
 private:
  string name;
  vector<vec3> vertices;

 public:
  Model(){};
  void addVertex(vec3 vertex);
  optional<Model> loadModel(const string& filename);
};